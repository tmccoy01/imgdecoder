//
// Created by Tanner McCoy on 12/7/21.
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include "imgutil.h"

int BufRead(void *value, unsigned char *buf, unsigned long *loc, int len)
{
    memcpy(value, &buf[*loc], len);
    *loc += len; //update location pointer

    return 0;
}

int DecodeImgHeader(Node *node, FILE **file, unsigned long memloc, unsigned short size)
{
    unsigned char buf[size];
    int bytes;
    unsigned long pos = 0;

    if (fseek(*file, memloc, SEEK_SET) != 0) //Jump to Header Location
    {
        fprintf(stderr, "ERROR: DecodeImgHeader: Failed to move to file position %ld\n", memloc);
        return -1;
    }
    if ((bytes = fread(buf, 1, size, *file)) != size)
    {
        fprintf(stderr, "ERROR: DecodeImgHeader: Read %d bytes, not %d\n", bytes, size);
        return -1;
    }

    BufRead(&node->header.next, buf, &pos, 4);
    BufRead(&node->header.prev, buf, &pos, 4);
    BufRead(&node->header.parent, buf, &pos, 4);
    BufRead(&node->header.child, buf, &pos, 4);
    BufRead(&node->header.dataPtr, buf, &pos, 4);
    BufRead(&node->header.dataSize, buf, &pos, 4);
    BufRead(&node->header.name, buf, &pos, 64);
    BufRead(&node->header.type, buf, &pos, 32);

    BufRead(&node->header.modTime, buf, &pos, 4);

    if (strcmp((char *) node->header.type, "root")==0)
        node->header.typeNum = ROOT;
    else if (strcmp((char *) node->header.type, "ImgFormatInfo831")==0)
        node->header.typeNum = IMGFORMATINFO831;
    else if (strcmp((char *) node->header.type, "Eimg_Layer")==0)
        node->header.typeNum = EIMG_LAYER;
    else if (strcmp((char *) node->header.type, "Edms_State")==0)
        node->header.typeNum = EDMS_STATE;
    else if (strcmp((char *) node->header.type, "Ehfa_Layer")==0)
        node->header.typeNum = EHFA_LAYER;
    else if (strcmp((char *) node->header.type, "Eimg_NonInitializedValue")==0)
        node->header.typeNum = EIMG_NONINITIALIZEDVALUE;
    else if (strcmp((char *) node->header.type, "Eprj_MapInfo")==0)
        node->header.typeNum = EPRJ_MAPINFO;
    else if (strcmp((char *) node->header.type, "Esta_Statistics")==0)
        node->header.typeNum = ESTA_STATISTICS;
    else
        node->header.typeNum = IMG_UNKNOWN; //Haven't defined some

    return 0;
}

int DecodeImgFormatInfo831(unsigned char buf[], ImgFormatInfo831 *node)
{
    unsigned long pos = 0;

    BufRead(&node->spaceUsed, buf, &pos, 4);

    return 0;
}

int DecodeEimg_Layer(unsigned char buf[], Eimg_Layer *node)
{
    unsigned long pos = 0;

    BufRead(&node->width, buf, &pos, 4);
    BufRead(&node->height, buf, &pos, 4);
    BufRead(&node->layerType, buf, &pos, 2);
    BufRead(&node->pixelType, buf, &pos, 2);
    BufRead(&node->blockWidth, buf, &pos, 4);
    BufRead(&node->blockHeight, buf, &pos, 4);

    return 0;
}

int DecodeEdms_State(unsigned char buf[], Edms_State *node)
{
    unsigned long vBlockCnt = 0, vBlockPtr = 0, pos = 0;
    int i;

    BufRead(&node->numVirtualBlocks, buf, &pos, 4);
    BufRead(&node->numObjectsPerBlock, buf, &pos, 4);
    BufRead(&node->nextObjectNum, buf, &pos, 4);
    BufRead(&node->compressionType, buf, &pos, 2);

    //get virtual block info
    BufRead(&vBlockCnt, buf, &pos, 4);
    BufRead(&vBlockPtr, buf, &pos, 4); //no need as data is next in current buffer

    //allocate memory for virtual blocks
    if ((node->blockInfo = malloc(sizeof(Edms_VirtualBlockInfo) * vBlockCnt)) == NULL)
    {
        fprintf(stderr, "ERROR: DecodeEdms_State: Failed to allocate memory\n");
        return -1;
    }
    memset(node->blockInfo, 0, sizeof(Edms_VirtualBlockInfo) * vBlockCnt);

    //assign data to virtual blocks
    for (i=0; i<vBlockCnt; i++)
    {
        BufRead(&node->blockInfo[i].fileCode, buf, &pos, 2);
        BufRead(&node->blockInfo[i].offset, buf, &pos, 4);
        BufRead(&node->blockInfo[i].size, buf, &pos, 4);
        BufRead(&node->blockInfo[i].logValid, buf, &pos, 2);
        BufRead(&node->blockInfo[i].compressionType, buf, &pos, 2);
    }
    // there is more to decode but its junk

    return 0;
}

int DecodeEhfa_Layer(unsigned char buf[], Ehfa_Layer *node)
{
    unsigned long pos = 0;

    BufRead(&node->type, buf, &pos, 2);
    BufRead(&node->dictionaryPtr, buf, &pos, 4);

    return 0;
}

int DecodeEimg_NonInitializedValue(unsigned char buf[], Eimg_NonInitializedValue *node)
{
    unsigned long blockCnt = 0, blockPtr = 0, pos = 0;

    BufRead(&blockCnt, buf, &pos, 4); //unused
    BufRead(&blockPtr, buf, &pos, 4); //unused
    BufRead(&node->valueBD.numRows, buf, &pos, 4);
    BufRead(&node->valueBD.numColumns, buf, &pos, 4);
    BufRead(&node->valueBD.dataType, buf, &pos, 2);
    BufRead(&node->valueBD.objectType, buf, &pos, 2);

    //need to decode data
    //the number of bytes determined by rows x columns x datatype
    //we are short circuiting this because we only see single double as value

    switch (node->valueBD.dataType)
    {
        case 0: //EGDA_TYPE_U1
            break;
        case 1: //EGDA_TYPE_U2
            break;
        case 2: //EGDA_TYPE_U4
            break;
        case 3: //EDGA_TYPE_U8
            break;
        case 4: //EDGA_TYPE_S8
            break;
        case 5: //EDGA_TYPE_U16
            break;
        case 6: //EDGA_TYPE_S16
            break;
        case 7: //EDGA_TYPE_U32
            break;
        case 8: //EDGA_TYPE_S32
            break;
        case 9: //EDGA_TYPE_F32
            break;
        case 10: //EDGA_TYPE_F64
            BufRead(&node->valueBD.data, buf, &pos, 8);
            break;
        case 11: //EDGA_TYPE_C64
            break;
        case 12: //EDGA_TYPE_C128
            break;
    }


    return 0;
}

int DecodeEprj_MapInfo(unsigned char buf[], Eprj_MapInfo *node)
{
    unsigned long nameCnt = 0, namePtr = 0, coordCnt = 0, coordPtr = 0, pos = 0;

    BufRead(&nameCnt, buf, &pos, 4);
    BufRead(&namePtr, buf, &pos, 4);

    BufRead(&node->proName, buf, &pos, nameCnt);

    BufRead(&coordCnt, buf, &pos, 4);
    BufRead(&coordPtr, buf, &pos, 4);

    BufRead(&node->upperLeftCenter.x, buf, &pos, 8);
    BufRead(&node->upperLeftCenter.y, buf, &pos, 8);

    BufRead(&coordCnt, buf, &pos, 4);
    BufRead(&coordPtr, buf, &pos, 4);

    BufRead(&node->lowerRightCenter.x, buf, &pos, 8);
    BufRead(&node->lowerRightCenter.y, buf, &pos, 8);

    BufRead(&coordCnt, buf, &pos, 4);
    BufRead(&coordPtr, buf, &pos, 4);

    BufRead(&node->pixelSize.x, buf, &pos, 8);
    BufRead(&node->pixelSize.y, buf, &pos, 8);

    return 0;
}

int DecodeEsta_Statistics(unsigned char buf[], Esta_Statistics *node)
{
    unsigned long pos = 0;

    BufRead(&node->minimum, buf, &pos, 8);
    BufRead(&node->maximum, buf, &pos, 8);
    BufRead(&node->mean, buf, &pos, 8);
    BufRead(&node->median, buf, &pos, 8);
    BufRead(&node->mode, buf, &pos, 8);
    BufRead(&node->stddev, buf, &pos, 8);

    return 0;
}

int DecodeImgNode(Node *node, FILE **file)
{
    unsigned char buf[node->header.dataSize];
    int bytes;

    if (node->header.dataSize == 0) //No data, probably root
        return 0;

    if (fseek(*file, node->header.dataPtr, SEEK_SET) != 0)
    {
        fprintf(stderr, "ERROR: DecodeImgNode: Failed to move to file position %ld\n", node->header.dataPtr);
        return -1;
    }
    if ((bytes = fread(buf, 1, node->header.dataSize, *file)) != node->header.dataSize)
    {
        fprintf(stderr, "ERROR: DecodeImgNode: Read %d bytes, not %ld\n", bytes, node->header.dataSize);
        return -1;
    }

    //Use a switch statement to call the appropriate decode routine
    switch (node->header.typeNum)
    {
        case IMGFORMATINFO831:
            if (DecodeImgFormatInfo831(buf, &node->ImgFormatInfo) != 0)
                return -1;
            break;
        case EIMG_LAYER:
            if (DecodeEimg_Layer(buf, &node->Layer_1) != 0)
                return -1;
            break;
        case EDMS_STATE:
            if (DecodeEdms_State(buf, &node->RasterDMS) != 0)
                return -1;
            break;
        case EHFA_LAYER:
            if (DecodeEhfa_Layer(buf, &node->EhfaLayer) != 0)
                return -1;
            break;
        case EIMG_NONINITIALIZEDVALUE:
            if (DecodeEimg_NonInitializedValue(buf, &node->NonInitValue) != 0)
                return -1;
            break;
        case EPRJ_MAPINFO:
            if (DecodeEprj_MapInfo(buf, &node->Map_Info) != 0)
                return -1;
            break;
        case ESTA_STATISTICS:
            if (DecodeEsta_Statistics(buf, &node->Statistics) != 0)
                return -1;
            break;
        default:
            break;

    }

    return 0;
}

int AllocateNodes(Node *node, FILE **file)
{
    unsigned long memloc = 0, pos = 0;
    unsigned char buf[256];
    unsigned short entry_header_size = 0;
    int bytes;

    //Set node to all zero
    memset(node, 0, sizeof(Node));

    //Read File Header
    if ((bytes = fread(buf, 1, IMGFILEHEADERSIZE, *file)) != IMGFILEHEADERSIZE)
    {
        fprintf(stderr, "ERROR: AllocateNodes: Read %d bytes, not %d\n", bytes, IMGFILEHEADERSIZE);
        return -1;
    }

    pos = 16;
    BufRead(&memloc, buf, &pos, 4);
    if (fseek(*file, memloc, SEEK_SET) != 0) //jump to location specified
    {
        fprintf(stderr, "ERROR: AllocateNodes: Failed to move to file position %ld\n", memloc);
        return -1;
    }

    //Read File Description
    if ((bytes = fread(buf, 1, IMGFILEDESCSIZE, *file)) != IMGFILEDESCSIZE)
    {
        fprintf(stderr, "ERROR: AllocateNodes: Read %d bytes, not %d\n", bytes, IMGFILEDESCSIZE);
        return -1;
    }

    pos = 8;
    BufRead(&memloc, buf, &pos, 4);
    BufRead(&entry_header_size, buf, &pos, 2);

    //Establish infinite loop for data decode until all node and children have been read
    //Start with Root Node
    //Read entire layer and store all child addresses in structure
    //Loop through children
    //Read entire layer and store all child addresses in same structure
    //Keep a counter that tells you which child you are working on

    node->childIndex = 0; //set child index
    node->numChildren = 0; //set num children to zero
    while (true)
    {
        if (DecodeImgHeader(node, file, memloc, entry_header_size) != 0) //fills in header
            return -1;

        //save the child location to child structure
        if (node->header.child != 0)
        {
            node->children[node->numChildren]= node->header.child;
            node->numChildren++;
        }

        if (DecodeImgNode(node, file) != 0)
            return -1;

        memloc = node->header.next;

        if (memloc==0)
        {
            // set memloc to first available child if no more on this layer
            if (node->childIndex != node->numChildren)
            {
                memloc = node->children[node->childIndex];
                node->childIndex++;
            }
            else
                break; //nothing else to look at
        }
    }
    return 0;
}

int DecodeVirtualBlockData(Edms_VirtualBlockInfo *blockInfo, FILE **file, float data_values[], unsigned long num_obj, double nonInit)
{
    int i, j, k, shiftval;
    unsigned char buf[blockInfo->size];
    unsigned long  min = 0, tmp_buf, num_segments = 0, data_offset = 0, pos = 0;
    unsigned char num_bits_per_value = 0, num_bytes_per_value, extra_count_bytes;
    unsigned long *data_counts;
    int bytes;
    float nonInitF;

    nonInitF = (float) nonInit;

    if (fseek(*file, blockInfo->offset, SEEK_SET) != 0) //jump to location specified
    {
        fprintf(stderr, "ERROR: DecodeVirtualBlockData: Failed to move to file position %ld\n", blockInfo->offset);
        return -1;
    }
    if ((bytes = fread(buf, 1, blockInfo->size, *file)) != blockInfo->size)
    {
        fprintf(stderr, "ERROR: DecodeVirtualBlockData: Read %d bytes, not %ld\n", bytes, blockInfo->size);
        return -1;
    }

    if (blockInfo->logValid!=1) //bad data
        return -1;

    memset(data_values, 0, sizeof(float) * num_obj); //clear out values

    if (blockInfo->compressionType==1) //compressed data
    {
        BufRead(&min, buf, &pos, 4);

        BufRead(&num_segments, buf, &pos, 4);
        BufRead(&data_offset, buf, &pos, 4);
        BufRead(&num_bits_per_value, buf, &pos, 1); //should be 8 or 32
        num_bytes_per_value = num_bits_per_value/8;

        if ((data_counts = malloc(sizeof(unsigned long) * num_segments)) == NULL)
        {
            fprintf(stderr, "ERROR: DecodeVirtualBlockData: Failed to allocate memory\n");
            return -1;
        }

        //get the data counts
        for (i=0;i<num_segments;i++)
        {
            extra_count_bytes  = (unsigned char) ((buf[pos] & 0xC0) >> 6);

            shiftval = extra_count_bytes * 8;
            data_counts[i] = (unsigned long) ((buf[pos] & 0x3F) << shiftval); pos++;
            for (j=extra_count_bytes;j>0;j--)
            {
                shiftval = (j-1) * 8;
                data_counts[i] = data_counts[i] | (unsigned long) (buf[pos] << shiftval);
                pos++;
            }
        }

        //get and assign the data values
        k=0;
        for (i=0;i<num_segments;i++)
        {
            tmp_buf = 0; //When compressed data values are "Big Endian"
            for (j=0;j<num_bytes_per_value;j++)
                tmp_buf = tmp_buf | (unsigned long) (buf[pos+j] << (8*(num_bytes_per_value-1-j)));

            tmp_buf += min;
            pos += num_bytes_per_value;

            for (j=0;j<data_counts[i];j++)
            {
                memcpy(&data_values[k], &tmp_buf, 4);
                k++;
            }
        }
        free(data_counts);
    }
    else
    {
        for (i=0;i<num_obj;i++)
            BufRead(&data_values[i], buf, &pos, 4);
    }

    for (i=0;i<num_obj;i++)
    {
        if (memcmp(&data_values[i], &nonInitF, 4)==0) //value is uninit
            data_values[i] = 0;
    }

    return 0;
}

int AssignPixels(float img[], Node *node, FILE **file)
{
    int i, j, k, row, column, ul_column, ul_row, data_index, img_index;
    float *data_values;

    if ((data_values = malloc(sizeof(float) * node->RasterDMS.numObjectsPerBlock)) == NULL)
    {
        fprintf(stderr, "ERROR: AssignPixels: Failed to allocate memory\n");
        return -1;
    }

    row = 0;
    column = 0;
    ul_column = 0;
    ul_row = 0;
    for (i=0;i<node->RasterDMS.numVirtualBlocks;i++)
    {
        if (DecodeVirtualBlockData(&node->RasterDMS.blockInfo[i], file, data_values, node->RasterDMS.numObjectsPerBlock, node->NonInitValue.valueBD.data) != 0)
            return -1;

        for (j=0;j<node->Layer_1.blockHeight;j++)
        {
            for (k=0;k<node->Layer_1.blockWidth;k++)
            {
                data_index = (j*node->Layer_1.blockWidth) + k;

                row = ul_row + j;
                column = ul_column + k;

                img_index = row * node->Layer_1.width + column;
                if (row<node->Layer_1.height && column<node->Layer_1.width)
                    img[img_index] = data_values[data_index];
            }
        }

        ul_column += node->Layer_1.blockWidth;
        if (ul_column >= node->Layer_1.width)
        {
            ul_column = 0;
            ul_row += node->Layer_1.blockHeight;
        }
    }

    free(data_values);
    return 0;
}




