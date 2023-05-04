#include "sd.h"
#include "mini_uart.h"
#include "printf.h"
#include "stdlib.h"

// get the end of bss segment from linker
extern unsigned char bss_end;

static unsigned int partitionlba = 0;

// the BIOS Parameter Block (in Volume Boot Record)
typedef struct {
    char            jmp[3];
    char            oem[8];
    unsigned char   bps0;
    unsigned char   bps1;
    unsigned char   spc;
    unsigned short  rsc;
    unsigned char   nf;
    unsigned char   nr0;
    unsigned char   nr1;
    unsigned short  ts16;
    unsigned char   media;
    unsigned short  spf16;
    unsigned short  spt;
    unsigned short  nh;
    unsigned int    hs;
    unsigned int    ts32;
    unsigned int    spf32;
    unsigned int    flg;
    unsigned int    rc;
    char            vol[6];
    char            fst[8];
    char            dmy[20];
    char            fst2[8];
} __attribute__((packed)) bpb_t;

// directory entry structure
typedef struct {
    char            name[8];
    char            ext[3];
    char            attr[9];
    unsigned short  ch;
    unsigned int    attr2;
    unsigned short  cl;
    unsigned int    size;
} __attribute__((packed)) fatdir_t;

/**
 * Get the starting LBA address of the first partition
 * so that we know where our FAT file system starts, and
 * read that volume's BIOS Parameter Block
 */
int fat_getpartition(void)
{
    unsigned char *mbr=&bss_end;
    bpb_t *bpb=(bpb_t*)&bss_end;
    // read the partitioning table
    if(sd_readblock(0,&bss_end,1)) {

        if(mbr[510]!=0x55 || mbr[511]!=0xAA) {
            printf("ERROR: Bad magic in MBR\n");
            return 0;
        }

        if(mbr[0x1C2]!=0xE/*FAT16 LBA*/ && mbr[0x1C2]!=0xC/*FAT32 LBA*/) {
            printf("ERROR: Wrong partition type\n");
            return 0;
        }
        partitionlba=mbr[0x1C6] + (mbr[0x1C7]<<8) + (mbr[0x1C8]<<16) + (mbr[0x1C9]<<24);
        // read the boot record
        if(!sd_readblock(partitionlba,&bss_end,1)) {
            printf("ERROR: Unable to read boot record\n");
            return 0;
        }

        if( !(bpb->fst[0]=='F' && bpb->fst[1]=='A' && bpb->fst[2]=='T') &&
            !(bpb->fst2[0]=='F' && bpb->fst2[1]=='A' && bpb->fst2[2]=='T')) {
            printf("ERROR: Unknown file system type\n");
            return 0;
        }
        return 1;
    }
    return 0;
}

/**
 * List root directory entries in a FAT file system
 */
void fat_listdirectory(void)
{
    bpb_t *bpb=(bpb_t*)&bss_end;
    fatdir_t *dir=(fatdir_t*)&bss_end;
    unsigned int root_sec, s;
    // find the root directory's LBA
    root_sec=((bpb->spf16?bpb->spf16:bpb->spf32)*bpb->nf)+bpb->rsc;
    s = (bpb->nr0 + (bpb->nr1 << 8));
    printf("FAT number of root diretory entries: ");
    printf("%x", s);
    s *= sizeof(fatdir_t);
    if(bpb->spf16==0) {
        // adjust for FAT32
        root_sec+=(bpb->rc-2)*bpb->spc;
    }
    // add partition LBA
    root_sec+=partitionlba;
    printf("\nFAT root directory LBA: ");
    printf("%x", root_sec);
    printf("\n");
    // load the root directory
    if(sd_readblock(root_sec,(unsigned char*)&bss_end,s/512+1)) {
        printf("\nAttrib    Cluster    Size    Name\n");
        // iterate on each entry and print out
        for(;dir->name[0]!=0;dir++) {
            // is it a valid entry?
            if(dir->name[0]==0xE5 || dir->attr[0]==0xF) continue;
            // decode attributes
            printf(dir->attr[0]& 1?"R":".");  // read-only
            printf(dir->attr[0]& 2?"H":".");  // hidden
            printf(dir->attr[0]& 4?"S":".");  // system
            printf(dir->attr[0]& 8?"L":".");  // volume label
            printf(dir->attr[0]&16?"D":".");  // directory
            printf(dir->attr[0]&32?"A":".");  // archive
            printf("    ");
            // staring cluster
            printf("%x", ((unsigned int)dir->ch)<<16|dir->cl);
            printf("        ");
            // size
            printf("%d", dir->size);
            printf("        ");
            // filename
            dir->attr[0]=0;
            printf(dir->name);
            printf("\n");
        }
    } else {
        printf("ERROR: Unable to load root directory\n");
    }
}

/*
   --> find a file in root directory
*/
unsigned int fat_getcluster(char *fn)
{
    bpb_t *bpb=(bpb_t*)&bss_end;
    fatdir_t *dir=(fatdir_t*)(&bss_end+512);
    unsigned int root_sec, s;
    // find the root directory's LBA
    root_sec=((bpb->spf16?bpb->spf16:bpb->spf32)*bpb->nf)+bpb->rsc;
    s = (bpb->nr0 + (bpb->nr1 << 8)) * sizeof(fatdir_t);
    if(bpb->spf16==0) {
        // adjust for FAT32
        root_sec+=(bpb->rc-2)*bpb->spc;
    }
    // add partition LBA
    root_sec+=partitionlba;
    // load the root directory
    if(sd_readblock(root_sec,(unsigned char*)dir,s/512+1)) {
        // iterate on each entry and check if it's the one we're looking for
        for(;dir->name[0]!=0;dir++) {
            // is it a valid entry?
            if(dir->name[0]==0xE5 || dir->attr[0]==0xF) continue;
            // filename match?
            if(!memcmp(dir->name,fn,11)) {
                printf("FAT File ");
                printf(fn);
                printf(" starts at cluster: ");
                printf("%x", ((unsigned int)dir->ch)<<16|dir->cl);
                printf("\n");
                // if so, return starting cluster
                return ((unsigned int)dir->ch)<<16|dir->cl;
            }
        }
        printf("ERROR: file not found\n");
    } else {
        printf("ERROR: Unable to load root directory\n");
    }
    return 0;
}

/*
 --> Read a whole file in memory
*/
char *fat_readfile(unsigned int cluster)
{
    // BIOS Parameter Block
    bpb_t *bpb=(bpb_t*)&bss_end;
    // File allocation tables. We choose between FAT16 and FAT32 dynamically
    unsigned int *fat32=(unsigned int*)(&bss_end+bpb->rsc*512);
    unsigned short *fat16=(unsigned short*)fat32;
    // Data pointers
    unsigned int data_sec, s;
    unsigned char *data, *ptr;
    // find the LBA of the first data sector
    data_sec=((bpb->spf16?bpb->spf16:bpb->spf32)*bpb->nf)+bpb->rsc;
    s = (bpb->nr0 + (bpb->nr1 << 8)) * sizeof(fatdir_t);
    if(bpb->spf16>0) {
        // adjust for FAT16
        data_sec+=(s+511)>>9;
    }
    // add partition LBA
    data_sec+=partitionlba;
    // dump important properties
    printf("FAT Bytes per Sector: ");
    printf("%x", (bpb->bps0 + (bpb->bps1 << 8)));
    printf("\nFAT Sectors per Cluster: ");
    printf("%x", bpb->spc);
    printf("\nFAT Number of FAT: ");
    printf("%x", bpb->nf);
    printf("\nFAT Sectors per FAT: ");
    printf("%x", (bpb->spf16?bpb->spf16:bpb->spf32));
    printf("\nFAT Reserved Sectors Count: ");
    printf("%x", bpb->rsc);
    printf("\nFAT First data sector: ");
    printf("%x", data_sec);
    printf("\n");
    // load FAT table
    s=sd_readblock(partitionlba+1,(unsigned char*)&bss_end+512,(bpb->spf16?bpb->spf16:bpb->spf32)+bpb->rsc);
    // end of FAT in memory
    data=ptr=&bss_end+512+s;
    // iterate on cluster chain
    while(cluster>1 && cluster<0xFFF8) {
        // load all sectors in a cluster
        sd_readblock((cluster-2)*bpb->spc+data_sec,ptr,bpb->spc);
        // move pointer, sector per cluster * bytes per sector
        ptr+=bpb->spc*(bpb->bps0 + (bpb->bps1 << 8));
        // get the next cluster in chain
        cluster=bpb->spf16>0?fat16[cluster]:fat32[cluster];
    }
    return (char*)data;
}
