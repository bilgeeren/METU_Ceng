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
#include <vector>
using namespace std;

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
#define BLOCK_OFFSET(block) ((block)*block_size)

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
    fd = open(argv[1],O_RDWR);

    lseek(fd, BASE_OFFSET,SEEK_SET);
    read(fd, &super, sizeof(super));

    block_size = 1024 << super.s_log_block_size;

    numberOfInodes = super.s_inodes_per_group;
    unsigned int argvBlock = atoi(argv[3]);
    unsigned int myBlockNumber = floor((argvBlock-1)/numberOfInodes);
    unsigned int myInodeOffset = argvBlock%numberOfInodes-1;
    unsigned int numberOfBlockGroup = ceil((double)super.s_blocks_count/super.s_blocks_per_group);
    unsigned int blocksPerGroup= super.s_blocks_per_group;
    unsigned int blockGroupSize = super.s_blocks_per_group*block_size;
    
    super.s_free_inodes_count -= 1;


    //*********************** READ GROUP DESC. TABLE INTO GROUPS ***************************
    struct ext2_group_desc groups[numberOfBlockGroup];

    for (int i = 0; i<numberOfBlockGroup; i++){
        lseek(fd,BASE_OFFSET+sizeof(ext2_group_desc)*(i)+sizeof(super), SEEK_SET);       
        read(fd, &groups[i], sizeof(ext2_group_desc));
    }
  
      
    if(isPath) {

       //PATH
    	unsigned int currInodeID = 2;
		unsigned int currGroup = 0;
    	struct ext2_inode currInode;
        unsigned int totalData = 0;
    	std::vector<string> myPath;
        std::string s = argv[3];
		std::string delimiter = "/";
		if(strcmp (argv[3], "/")){
			size_t pos = 0;
			pos = s.find(delimiter);
			if(pos == 0) {
				s.erase(0, pos + delimiter.length());
			}		
			pos = 0;
			std::string token;
			while ((pos = s.find(delimiter)) != std::string::npos) {
			    token = s.substr(0, pos);
			    myPath.push_back(token);
			    s.erase(0, pos + delimiter.length());
			}
			token = s;
			myPath.push_back(token);





			for(int j = 0;j<myPath.size();j++){
				lseek(fd, BLOCK_OFFSET(groups[currGroup].bg_inode_table)+sizeof(struct ext2_inode)*(currInodeID-1), SEEK_SET);
	            read(fd, &currInode, sizeof(struct ext2_inode));

	            for(int k = 0; k<15;k++) {
	            	unsigned int absAddress = currInode.i_block[k];
		            totalData = 0;
		            if(absAddress != 0){
		            	unsigned int   _inode;     
			            unsigned short _rec_len;  
		                unsigned char  _name_len;  
		                while(totalData  <block_size ) {
		            

			                lseek(fd, (absAddress)*block_size +totalData,SEEK_SET );
			                read(fd,&_inode,sizeof(unsigned int));
			                read(fd,&_rec_len,sizeof(unsigned short));
			                read(fd,&_name_len,sizeof(unsigned char));
			                lseek(fd, (absAddress)*block_size +totalData+sizeof(unsigned int)+sizeof(unsigned short)+sizeof(unsigned char)*2,SEEK_SET );
			                int _name_len_ = _name_len;
			                char _name[_name_len_+1];
			                for(int i = 0;i<_name_len_;i++){
					        	read(fd,&_name[i],sizeof(char));

					        }

			                _name[_name_len_]='\0';


			                if(!strcmp (myPath[k].c_str(), _name) ) {
			                	currInodeID = _inode;

			                	currGroup = _inode/super.s_inodes_per_group;
			                	break;
			                }
			                totalData += _rec_len;

		                }
		            }
	            }

			}
		}


		argvBlock = currInodeID;
        myBlockNumber = currGroup;
        myInodeOffset = argvBlock%numberOfInodes-1;
        numberOfBlockGroup = ceil((double)super.s_blocks_count/super.s_blocks_per_group);
        blocksPerGroup= super.s_blocks_per_group;


    }

      bool foundEmpty = false;


    //************************************* FIND EMPTY INODE *****************************************
     bmap inode_bitmap[block_size];
    for(int j = 0; j <numberOfBlockGroup; j++){
        lseek(fd, BLOCK_OFFSET(groups[j].bg_inode_bitmap), SEEK_SET);

        read(fd, inode_bitmap, block_size);

        for(int i = 1; i<numberOfInodes; i++){
            
           
            if(!BM_ISSET(i-1, inode_bitmap)){   

                BM_SET(i-1,inode_bitmap);
                lseek(fd, BLOCK_OFFSET(groups[j].bg_inode_table)+sizeof(struct ext2_inode)*(i-1), SEEK_SET);
            	read(fd, &emptyNode, sizeof(struct ext2_inode));
                // -------------MANIPULATE INODE BIT MAP -----------
                lseek(fd, BLOCK_OFFSET(groups[j].bg_inode_bitmap), SEEK_SET);
                write(fd, &inode_bitmap, block_size);
                // --------------------------------------------------
               	groups[j].bg_free_inodes_count -= 1;
               	foundEmpty = true;
                emptyNodeId = i;
                emptyNodeGroupId = j;
                break;
            }
        
         }
         if(foundEmpty) {
         	break;
         }
    }
    foundEmpty = false;
    cout<<emptyNodeGroupId*numberOfInodes+emptyNodeId<<" ";


    //******************************* FIND DESTINATION INODE *****************************************
    lseek(fd, BLOCK_OFFSET(groups[myBlockNumber].bg_inode_table)+myInodeOffset*sizeof(ext2_inode),SEEK_SET);
    read(fd, &destNode, sizeof(destNode));

    struct stat source;
    stat(argv[2],&source);
    emptyNode.i_mode = source.st_mode;
    emptyNode.i_uid = source.st_uid;
    emptyNode.i_gid = source.st_gid;
    emptyNode.i_size = source.st_size; 
    emptyNode.i_blocks = ceil(source.st_size/(float) block_size)*block_size/512;
    emptyNode.i_atime = source.st_atime;
    emptyNode.i_mtime = source.st_mtime;
    emptyNode.i_ctime = source.st_ctime;
    emptyNode.i_links_count = 1;
    // ************************************ DIR ENTRY SETTED *******************************************

    struct ext2_dir_entry myDirEntry;
    myDirEntry.inode = emptyNodeGroupId*numberOfInodes + emptyNodeId;
    myDirEntry.rec_len = sizeof(unsigned int)+ sizeof(unsigned short)+sizeof(unsigned char)+sizeof(unsigned char)+sizeof(char)*strlen(argv[2]);
    myDirEntry.name_len = strlen(argv[2]);
    myDirEntry.file_type = 1;

    for(int i = 0; i<strlen(argv[2]);i++) {
        myDirEntry.name[i] = argv[2][i];
    }

    // ******************************** FIND PLACE DIR ENTRY *********************************************

    unsigned int myNodeBlockNumber;
    
    unsigned int destOffset = BASE_OFFSET+myBlockNumber*sizeof(ext2_group_desc)+myInodeOffset*sizeof(ext2_inode);
    unsigned int totalData = 0;
    unsigned int destBlock;
    unsigned char emptyBlock[block_size];
    int emptyBlockId ;
    int emptyBlockGroupId ;
    bool isAvaible = false;

    for (int i = 0; i<15; i++) {
 		bool found = false;
        totalData = 0;
        unsigned int reallastData = 0;
        unsigned int lastrecLen = 0;
     	unsigned int absAddress = destNode.i_block[i];
        if(absAddress != 0){
        	unsigned int   _inode;     
            unsigned short _rec_len;  
            unsigned char  _name_len;  
            while((totalData + myDirEntry.rec_len) < block_size ) {
        

                lseek(fd, (absAddress)*block_size +totalData,SEEK_SET );
                read(fd,&_inode,sizeof(unsigned int));
                read(fd,&_rec_len,sizeof(unsigned short));
                read(fd,&_name_len,sizeof(unsigned char));
                int _name_len_ = _name_len ;
                _name_len_ = (_name_len_/4+1)*4;

                totalData += _rec_len;
                lastrecLen = _rec_len;
                reallastData = sizeof(unsigned int)+sizeof(unsigned short)+sizeof(unsigned char)*2 + _name_len_*sizeof(char);
                

            }
            if(reallastData != lastrecLen){
                if(lastrecLen-reallastData > myDirEntry.rec_len){
                    emptyBlockId = absAddress;
                    emptyBlockGroupId = i;
                    totalData = totalData-_rec_len;
                    isAvaible = true;
                    // -----------------MANIPULATE REC LEN -----------
               		lseek(fd, (absAddress)*block_size +totalData+sizeof(unsigned int),SEEK_SET );
                    write(fd, &reallastData, sizeof(unsigned short));
                    myDirEntry.rec_len = block_size -(totalData+reallastData);
					

                    // ------------------------------------------------
                    totalData += reallastData; 
                    destBlock = absAddress;
                    break;
                }
            }    

        }
        else{
            //*********************EMPTY IBLOCK*******************************

            bmap block_bitmap[block_size];
 			for(int j = 0; j<numberOfBlockGroup; j++) {
        		lseek(fd, BLOCK_OFFSET(groups[j].bg_block_bitmap), SEEK_SET);
        		read(fd, block_bitmap, block_size);
        		for(int k = 1; k<blocksPerGroup;k++) {

        			if(!BM_ISSET(k-super.s_first_data_block, block_bitmap)) {
        				BM_SET(k-super.s_first_data_block ,block_bitmap);
        				lseek(fd, BLOCK_OFFSET(groups[j].bg_block_bitmap), SEEK_SET);
        				write(fd, &block_bitmap, block_size);
        				destBlock = i;
        				emptyBlockId = k;
        				emptyBlockGroupId = j;
        				super.s_free_blocks_count -=1;
    					groups[j].bg_free_blocks_count -= 1;
        				found = true;
        				destNode.i_block[i] = emptyBlockGroupId*blocksPerGroup + emptyBlockId;
        				destNode.i_size += block_size;
        				destNode.i_blocks +=1;


        				break;
        			}
        		}
        		if(found) {
        			break;
        		}
        	}  

           if(found) {
        			break;
        		}

        }			
    }


    // ******************************** PLACE DIR ENTRY *********************************************

    if(isAvaible) {
        unsigned int totalBlockNumber =  emptyBlockId;
        lseek(fd, totalBlockNumber*block_size+totalData, SEEK_SET );
        write(fd,&myDirEntry.inode,sizeof(unsigned int));
        write(fd,&myDirEntry.rec_len,sizeof(unsigned short));
        write(fd,&myDirEntry.name_len,sizeof(unsigned char));
        write(fd,&myDirEntry.file_type,sizeof(unsigned char));
        for(int i = 0;i<myDirEntry.name_len;i++){
        	write(fd,&myDirEntry.name[i],sizeof(char));

        }

    }
    else {
        unsigned int totalBlockNumber = emptyBlockGroupId*blocksPerGroup+ emptyBlockId;
        lseek(fd, totalBlockNumber*block_size, SEEK_SET );
        myDirEntry.rec_len = block_size;
        
        
        write(fd,&myDirEntry.inode,sizeof(unsigned int));
        write(fd,&myDirEntry.rec_len,sizeof(unsigned short));
        write(fd,&myDirEntry.name_len,sizeof(unsigned char));
        write(fd,&myDirEntry.file_type,sizeof(unsigned char));
        for(int i = 0;i<int(myDirEntry.name_len);i++){
        	write(fd,&myDirEntry.name[i],sizeof(char));

        }

        destNode.i_block[destBlock] = totalBlockNumber+1;
    }  



    // ******************** DIR ENTRY PLACED *************************************
    int fdSource = open(argv[2],O_RDWR);
    unsigned char temp[block_size];
    unsigned int tempBlockId;
    unsigned int tempBlockGroupId;
    bmap block_bitmap[block_size];

    for(int i = 0; i<ceil((emptyNode.i_size)/(float)block_size);i++){
        //*********************EMPTY IBLOCK*******************************
    	lseek(fdSource, i*block_size, SEEK_SET);
        read(fdSource,temp,block_size);
        if(i<12) {
        	if(i != 0) {
        		cout<<",";
        	}
        	for(int j = 0; j<numberOfBlockGroup; j++) {
        		bool found = false;
        		lseek(fd, BLOCK_OFFSET(groups[j].bg_block_bitmap), SEEK_SET);
        		read(fd, block_bitmap, block_size);
        		for(int k = 1; k<blocksPerGroup;k++) {
        			if(!BM_ISSET(k-super.s_first_data_block, block_bitmap)) {
        				BM_SET(k-super.s_first_data_block ,block_bitmap);
        				lseek(fd, BLOCK_OFFSET(groups[j].bg_block_bitmap), SEEK_SET);
        				write(fd, &block_bitmap, block_size);
        				unsigned int total = j*blocksPerGroup+k;
        				emptyNode.i_block[i] = total;
        				super.s_free_blocks_count -=1;
        				groups[j].bg_free_blocks_count -= 1;
        				lseek(fd,  total*block_size, SEEK_SET);			
        				write(fd, &temp, block_size);
        				found = true;
        				cout<<total;



        				break;

        			}
        		}
        		if(found) {

        			break;
        		}

        	}
    	}  
    	else{
    		//cout<<"image büyük"<<endl;
    		break;
    	}     
    }
    lseek(fd, BLOCK_OFFSET(groups[emptyNodeGroupId].bg_inode_table)+sizeof(struct ext2_inode)*(emptyNodeId-1), SEEK_SET);
    write(fd, &emptyNode, sizeof(struct ext2_inode));
    lseek(fd, BASE_OFFSET,SEEK_SET);
    write(fd, &super, sizeof(super));
    for (int i = 0; i<numberOfBlockGroup; i++){
        lseek(fd,BASE_OFFSET+sizeof(ext2_group_desc)*(i)+sizeof(super), SEEK_SET);       
        write(fd, &groups[i], sizeof(ext2_group_desc));
    }
    lseek(fd, BLOCK_OFFSET(groups[myBlockNumber].bg_inode_table)+myInodeOffset*sizeof(ext2_inode),SEEK_SET);
    write(fd, &destNode, sizeof(destNode));


    close(fd);
    return 0;
}
