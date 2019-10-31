#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <math.h>
#include "ext21.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

#define BASE_OFFSET 1024
#define EXT2_BLOCK_SIZE 1024
#define IMAGE "image.img"

typedef unsigned char bmap;
#define __NBITS (8 * (int) sizeof (bmap))
#define __BMELT(d) ((d) / __NBITS)
#define __BMMASK(d) ((bmap) 1 << ((d) % __NBITS))
#define BM_SET(d, set) ((set[__BMELT (d)] |= __BMMASK (d)))
#define BM_CLR(d, set) ((set[__BMELT (d)] &= ~__BMMASK (d)))
#define BM_ISSET(d, set) ((set[__BMELT (d)] & __BMMASK (d)) != 0)

unsigned int block_size = 0;
#define BLOCK_OFFSET(block) (BASE_OFFSET + (block-1)*block_size)

int main(int argc, char* argv[])
{
    struct ext2_super_block super;
    
    struct ext2_inode destNode;
    struct ext2_inode emptyNode;
    int emptyNodeId;
    int emptyNodeGroupId;

    int fd;
    unsigned int numberOfInodes;
    bool isPath = false ;

    for(int i=0; i<strlen(argv[3]); i++) {
        if(!isdigit(argv[3][i])){
            isPath = true;
            break;
        }
    }

    if(!isPath) {


        //******************************* READ SUPER *****************************************
        fd = open(argv[1],O_RDWR);

        lseek(fd, BASE_OFFSET,SEEK_SET);
        read(fd, &super, sizeof(super));

        block_size = 1024 ;

        numberOfInodes = super.s_inodes_per_group;
        unsigned int argvBlock = atoi(argv[3]);
        unsigned int myBlockNumber = floor((argvBlock-1)/numberOfInodes);
        unsigned int myInodeOffset = argvBlock%numberOfInodes-1;
        unsigned int numberOfBlockGroup = ceil((double)super.s_blocks_count/super.s_blocks_per_group);
        

        //*********************** READ GROUP DESC. TABLE INTO GROUPS ***************************
        struct ext2_group_desc groups[numberOfBlockGroup];

        for (int i = 0; i<numberOfBlockGroup; i++){
            lseek(fd,BASE_OFFSET+sizeof(ext2_group_desc)*(i)+sizeof(super), SEEK_SET);       
            read(fd, &groups[i], sizeof(ext2_group_desc));
        }
  
        block_size = 1024 << super.s_log_block_size;
      
        std::cout<<"block size "<<block_size<<std::endl;
        std::cout<<"block Number: "<<myBlockNumber<<std::endl;
        std::cout<<"myInodeOffset: "<<myInodeOffset<<std::endl;
        std::cout<<"s_inodes_per_group: "<<super.s_inodes_per_group<<std::endl;

        unsigned int blockGroupSize = super.s_blocks_per_group*block_size;
        bool foundEmpty = false;

        std::cout<<"numBlockGroups: "<<numberOfBlockGroup<<std::endl;

        //************************************* FIND EMPTY INODE *****************************************
        for(int j = 0; j <numberOfBlockGroup; j++){
            bmap* inode_bitmap = (bmap*)malloc(block_size);
            lseek(fd, BLOCK_OFFSET(groups[j].bg_inode_bitmap), SEEK_SET);
            read(fd, inode_bitmap, block_size);

            for(int i = 1; i<numberOfInodes; i++){
                
                lseek(fd, BLOCK_OFFSET(groups[j].bg_inode_table)+sizeof(struct ext2_inode)*(i-1), SEEK_SET);
                read(fd, &emptyNode, sizeof(struct ext2_inode));

                if(!BM_ISSET(i-1, inode_bitmap)){   
                    BM_SET(i-1,inode_bitmap);
                    // -------------MANIPULATE INODE BIT MAP -----------
                    std::cout<<"I NODE BIT MAP MANIPUlATION"<<groups[j].bg_inode_bitmap<<std::endl;
                    lseek(fd, BLOCK_OFFSET(groups[j].bg_inode_bitmap), SEEK_SET);
                    write(fd, &inode_bitmap, block_size);
                    // --------------------------------------------------
                    super.s_free_inodes_count -= 1;
                    lseek(fd, BASE_OFFSET,SEEK_SET);
                    write(fd, &super, sizeof(super));

                    emptyNodeId = i;
                    emptyNodeGroupId = j;
                    break;
                }
            
             }
        }


        std::cout<<"emptyNodeId: "<<emptyNodeId<<std::endl;
        std::cout<<"emptyNodeGroupId: "<<emptyNodeGroupId<<std::endl;
        std::cout<<"emptyNodeSize: "<<emptyNode.i_size<<std::endl;
        std::cout<<"normal inode size: "<<sizeof(ext2_inode)<<std::endl;
        return 0;
        //******************************* FIND DESTINATION INODE *****************************************
        lseek(fd, BLOCK_OFFSET(groups[myBlockNumber].bg_inode_table)+myInodeOffset*sizeof(ext2_inode),SEEK_SET);
        read(fd, &destNode, sizeof(destNode));
        
        std::cout<<"dest size: "<<destNode.i_size<<std::endl;
        std::cout<<"dest mode: "<<destNode.i_mode<<std::endl;
        std::cout<<"abs------- "<<destNode.i_block[0]<<std::endl;

        struct stat source;
        emptyNode.i_mode = source.st_mode;
        emptyNode.i_uid = source.st_uid;
        emptyNode.i_gid = source.st_gid;
        emptyNode.i_size = source.st_size;  // BU HATALI !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        emptyNode.i_blocks = source.st_blocks;
        emptyNode.i_atime = source.st_atime;
        emptyNode.i_mtime = source.st_mtime;
        emptyNode.i_ctime = source.st_ctime;

        lseek(fd, BLOCK_OFFSET(groups[emptyNodeGroupId].bg_inode_table)+sizeof(struct ext2_inode)*(emptyNodeId-1), SEEK_SET);
        write(fd, &emptyNode, sizeof(struct ext2_inode));
        // ************************************ DIR ENTRY SETTED *******************************************

        struct ext2_dir_entry myDirEntry;
        myDirEntry.inode = emptyNodeId;
        myDirEntry.rec_len = sizeof(unsigned int)+ sizeof(unsigned short)+sizeof(unsigned char)+sizeof(unsigned char)+sizeof(char)*strlen(argv[2]);
        myDirEntry.name_len = strlen(argv[2]);
        myDirEntry.file_type = 1;

        for(int i = 0; i<strlen(argv[2]);i++) {
            myDirEntry.name[i] = argv[2][i];
        }

        // ******************************** FIND PLACE DIR ENTRY *********************************************

        unsigned int myNodeBlockNumber;
        
        unsigned int destOffset = BASE_OFFSET+myBlockNumber*sizeof(ext2_group_desc)+myInodeOffset*sizeof(ext2_inode);
     	std::cout<<myDirEntry.rec_len<<std::endl;
        unsigned int totalData = 0;
        unsigned char* emptyBlock = (unsigned char*)malloc(block_size);
        int emptyBlockId ;
        int emptyBlockGroupId ;
        bool isAvaible = false;
        for (int i = 0; i<15; i++) {
        	//std::cout<<"FILES GONE WILD "<<std::endl;
            totalData = 0;
            unsigned int reallastData = 0;
            unsigned int lastrecLen = 0;
         	unsigned int absAddress = destNode.i_block[i];
            std::cout<<"abs"<<absAddress<<std::endl;
            if(absAddress != 0){
                while((totalData + myDirEntry.rec_len) <1024 ) {
            
                unsigned int   _inode;     
                unsigned short _rec_len;  
                unsigned char  _name_len;  
                unsigned char  file_type;
                lseek(fd, absAddress*block_size ,SEEK_CUR );
                read(fd,&_inode,sizeof(unsigned int));
                lseek(fd, sizeof(unsigned int),SEEK_CUR );
                read(fd,&_rec_len,sizeof(unsigned short));
                lseek(fd,sizeof(unsigned short),SEEK_CUR );
                read(fd,&_name_len,sizeof(unsigned char));
                lseek(fd, sizeof(unsigned char)*2 + _name_len*sizeof(char),SEEK_CUR );

                totalData += _rec_len;
                lastrecLen = _rec_len;
                reallastData = sizeof(unsigned int)+sizeof(unsigned short)+sizeof(unsigned char)*2 + _name_len*sizeof(char);
                }
                if(reallastData != lastrecLen){
                    if(lastrecLen-reallastData > myDirEntry.rec_len){
                        emptyBlockId = i;
                        emptyBlockGroupId = myBlockNumber;
                        totalData = totalData-lastrecLen+reallastData;
                        isAvaible = true;
                        // -----------------MANIPULATE REC LEN -----------
                        lseek(fd, absAddress*block_size +sizeof(unsigned int) ,SEEK_SET );
                        write(fd, &reallastData, sizeof(unsigned short));
                        // ------------------------------------------------
                        break;
                    }
                }     
            }
            else{
                std::cout<<"DATA BLOCK BUL"<<std::endl;
                //*********************EMPTY IBLOCK*******************************
                for(int j = 0; j <numberOfBlockGroup; j++){
                bmap* block_bitmap = (bmap*)malloc(block_size);
                lseek(fd, BLOCK_OFFSET(groups[j].bg_inode_bitmap), SEEK_SET);
                read(fd, block_bitmap, block_size);
                unsigned int initial = groups[j].bg_inode_bitmap%super.s_inodes_per_group +super.s_inodes_per_group*super.s_inode_size/block_size;
                for(int i = initial; i<super.s_blocks_per_group; i++){
                    
                    lseek(fd, BLOCK_OFFSET(groups[j].bg_block_bitmap)+block_size*(i), SEEK_SET);
                    read(fd, &emptyBlock, block_size);

                    if(!BM_ISSET(i, block_bitmap)){   
                        BM_SET(i,block_bitmap);

                        emptyBlockId = i;
                        emptyBlockGroupId = j;
                        unsigned int totalBlockNumber = emptyBlockGroupId*super.s_blocks_per_group + emptyBlockId;
                        lseek(fd, BLOCK_OFFSET(groups[j].bg_block_bitmap)+(i), SEEK_SET);
                        write(fd, &totalBlockNumber,sizeof(unsigned int));
                        break;
                    }                    
                }
             }   
               
            }			
        }

        // ******************************** PLACE DIR ENTRY *********************************************

        if(isAvaible) {
            std::cout<<"YAA HAK! "<<std::endl;
            lseek(fd, totalData, SEEK_SET);
            write(fd, &myDirEntry.inode, sizeof(unsigned int));
            lseek(fd, sizeof(unsigned int),SEEK_CUR );
            write(fd, &myDirEntry.rec_len,sizeof(unsigned short));
            lseek(fd,sizeof(unsigned short),SEEK_CUR );
            write(fd, &myDirEntry.name_len,sizeof(unsigned char));
            lseek(fd, sizeof(unsigned char), SEEK_CUR);
            write(fd, &myDirEntry.file_type,sizeof(unsigned char));
            lseek(fd, sizeof(unsigned char), SEEK_CUR);
            for(int i = 0; i<myDirEntry.name_len; i++){
                write(fd, &myDirEntry.name[i], sizeof(char));
                lseek(fd, sizeof(char), SEEK_CUR);
            }

        }   
        else {
            unsigned int totalBlockNumber = emptyBlockGroupId*super.s_blocks_per_group + emptyBlockId;
            lseek(fd, totalBlockNumber*block_size, SEEK_CUR );
            write(fd, &myDirEntry.inode, sizeof(unsigned int));
            lseek(fd, sizeof(unsigned int),SEEK_CUR );
            write(fd, &myDirEntry.rec_len,sizeof(unsigned short));
            lseek(fd,sizeof(unsigned short),SEEK_CUR );
            write(fd, &myDirEntry.name_len,sizeof(unsigned char));
            lseek(fd, sizeof(unsigned char), SEEK_CUR);
            write(fd, &myDirEntry.file_type,sizeof(unsigned char));
            lseek(fd, sizeof(unsigned char), SEEK_CUR);
            for(int i = 0; i<myDirEntry.name_len; i++){
                write(fd, &myDirEntry.name[i], sizeof(char));
                lseek(fd, sizeof(char), SEEK_CUR);
            }
        }   
        return 0;
        // ******************** DIR ENTRY PLACED *************************************
        int fdSource = open(argv[2],O_RDWR);
        unsigned char *temp[destNode.i_size/block_size/8];
        unsigned int tempBlockId = i;
        unsigned int tempBlockGroupId = j;
        for(int i = 0; block_size*i<destNode.i_size;i++){
                //*********************EMPTY IBLOCK*******************************
                for(int j = 0; j*8<block_size;j++) {
                    read(fdSource,temp[j],sizeof(unsigned char));
                }
                for(int j = 0; j <numberOfBlockGroup; j++){
                bmap* block_bitmap = (bmap*)malloc(block_size);
                lseek(fd, BLOCK_OFFSET(groups[j].bg_inode_bitmap), SEEK_SET);
                read(fd, block_bitmap, block_size);
                unsigned int initial = groups[j].bg_inode_bitmap%super.s_inodes_per_group +super.s_inodes_per_group*super.s_inode_size/block_size;
                if( i <12){
                    for(int i = initial; i<super.s_blocks_per_group; i++){
                        
                        lseek(fd, BLOCK_OFFSET(groups[j].bg_block_bitmap)+block_size*(i), SEEK_SET);
                        read(fd, &emptyBlock, block_size);

                        if(!BM_ISSET(i, block_bitmap)){   
                            BM_SET(i,block_bitmap);

                            tempBlockId = i;
                            tempBlockGroupId = j;
                            unsigned int totalBlockNumber = emptyBlockGroupId*super.s_blocks_per_group + emptyBlockId;
                            lseek(fd, BLOCK_OFFSET(groups[j].bg_block_bitmap)+(i), SEEK_SET);
                            write(fd, &totalBlockNumber,sizeof(unsigned int));
                            
                            for(int j = 0; j*8<block_size;j++) {
                                lseek(fd, totalBlockNumber*block_size+j*8, SEEK_SET );
                                write(fd, &temp[j],sizeof(unsigned char));
                            }
                            break;
                        }                    
                    }
                    // **** BLOCK NUMARASINI INODEDAKİ IBLOCKA YAZ
                }
                else {
                    break;
                }
                

            
        }
        // empty INODEU GERI YERİNE YAZ 
        //** YAZDIK DİYE UMUYORUZ :D

 }  


    }
    else {
        //PATH 

    }


    close(fd);
    return 0;
}
