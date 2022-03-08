//
// Created by Tanner McCoy on 12/7/21.
//

#pragma once

//Variable Definitions
#define ROOT                            1
#define IMGFORMATINFO831				2
#define EIMG_LAYER						3
#define EDMS_STATE						4
#define EHFA_LAYER						5
#define EIMG_NONINITIALIZEDVALUE		6
#define EPRJ_MAPINFO					7
#define ESTA_STATISTICS					8
#define IMG_UNKNOWN                     99

#define IMGFILEHEADERSIZE 20
#define IMGFILEDESCSIZE   18

//Structure Definition
typedef struct
{
    unsigned long  next;
    unsigned long  prev;
    unsigned long  parent;
    unsigned long  child;
    unsigned long  dataPtr;
    unsigned long  dataSize;
    unsigned char  name[64];
    unsigned char  type[32];
    unsigned short typeNum; //added for switch
    unsigned long  modTime;
}
        Ehfa_Entry; //this is essentially a node header

typedef struct
{
    unsigned long spaceUsed; //The approx space used by raster data in file
}
        ImgFormatInfo831;

typedef struct
{
    unsigned long width;
    unsigned long height;
    unsigned short layerType;
    unsigned short pixelType;
    unsigned long blockWidth;
    unsigned long blockHeight;
}
        Eimg_Layer;

typedef struct
{
    unsigned short fileCode; //always should be zero
    unsigned long offset; //points to data
    unsigned long size;
    unsigned short logValid; //1=data valid, 0=data invalid
    unsigned short compressionType; //1=compressed, 0=not compressed
}
        Edms_VirtualBlockInfo;

typedef struct
{
    unsigned long min;
    unsigned long max;
}
        Edms_FreeIDList;

typedef struct
{
    unsigned long numVirtualBlocks;
    unsigned long numObjectsPerBlock;
    unsigned long nextObjectNum; //unused gibberish
    unsigned short compressionType;
    Edms_VirtualBlockInfo *blockInfo;
    Edms_FreeIDList freeList; //not being used
    unsigned long modTime;
}
        Edms_State;

typedef struct
{
    unsigned short type; //0=raster, 1=vector
    unsigned long dictionaryPtr; //points to dictionary
}
        Ehfa_Layer;

typedef struct
{
    unsigned long numRows;
    unsigned long numColumns;
    unsigned short dataType;
    unsigned short objectType;
    double data; //assuming its a double (only type seen)
}
        Emif_T_BaseData;

typedef struct
{
    Emif_T_BaseData valueBD;
}
        Eimg_NonInitializedValue;

typedef struct
{
    double x;
    double y;
}
        Eprj_Coord_Size;

typedef struct
{
    unsigned char proName[256]; //Make it big enough to hold unknown value
    Eprj_Coord_Size upperLeftCenter;
    Eprj_Coord_Size lowerRightCenter;
    Eprj_Coord_Size pixelSize;
}
        Eprj_MapInfo;

typedef struct
{
    double minimum;
    double maximum;
    double mean;
    double median;
    double mode;
    double stddev;
}
        Esta_Statistics;

typedef struct
{
    Ehfa_Entry						header;
    ImgFormatInfo831				ImgFormatInfo;
    Eimg_Layer						Layer_1;
    Edms_State						RasterDMS;
    Ehfa_Layer						EhfaLayer;
    Eimg_NonInitializedValue		NonInitValue;
    Eprj_MapInfo					Map_Info;
    Esta_Statistics					Statistics;
    unsigned long 					children[256];
    unsigned short					numChildren;
    unsigned short					childIndex;
}
        Node;


//Function Prototypes
int BufRead(void *value, unsigned char *buf, unsigned long *loc, int len);
int DecodeImgHeader(Node *node, FILE **file, unsigned long memloc, unsigned short size);
int DecodeImgFormatInfo831(unsigned char buf[], ImgFormatInfo831 *node);
int DecodeEimg_Layer(unsigned char buf[], Eimg_Layer *node);
int DecodeEdms_State(unsigned char buf[], Edms_State *node);
int DecodeEhfa_Layer(unsigned char buf[], Ehfa_Layer *node);
int DecodeEimg_NonInitializedValue(unsigned char buf[], Eimg_NonInitializedValue *node);
int DecodeEprj_MapInfo(unsigned char buf[], Eprj_MapInfo *node);
int DecodeEsta_Statistics(unsigned char buf[], Esta_Statistics *node);
int DecodeImgNode(Node *node, FILE **file);
int AllocateNodes(Node *node, FILE **file);
int DecodeVirtualBlockData(Edms_VirtualBlockInfo *blockInfo, FILE **file, float data_values[], unsigned long num_obj, double nonInit);
int AssignPixels(float img[], Node *node, FILE **file);
