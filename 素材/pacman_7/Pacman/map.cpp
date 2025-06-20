#include "map.h"

#define N 3
#define A 2
#define B 1
#define O 0
#define S -1
const int Stage_1::initData[MAPLENGTH][MAPLENGTH] = {
    /*  B, B, B, B, B, B, B, B, B, A, B, B, B, B, B, B, B, B, B,
            B, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, B,
            B, A, A, B, A, A, B, B, B, A, B, B, B, A, A, B, A, A, B,
            B, A, B, B, A, A, A, A, A, A, A, A, A, A, A, B, B, A, B,
            B, A, B, A, A, A, B, B, B, A, B, B, B, A, A, A, B, A, B,
            B, A, B, A, A, A, A, A, A, A, A, A, A, A, A, A, B, A, B,
            B, A, A, A, A, A, B, B, A, A, A, B, B, A, A, A, A, A, B,
            B, A, B, A, A, A, A, A, A, A, A, A, A, A, A, A, B, A, B,
            B, A, B, A, A, A, A, A, B, A, B, A, A, A, A, A, B, A, B,
            A, A, A, A, A, A, A, A, B, B, B, A, A, A, A, A, A, A, A,
            B, A, B, A, A, A, A, A, A, A, A, A, A, A, A, A, B, A, B,
            B, A, B, A, A, B, A, A, A, A, A, A, A, B, A, A, B, A, B,
            B, A, B, A, B, B, B, A, A, A, A, A, B, B, B, A, B, A, B,
            B, A, A, A, A, B, A, A, A, A, A, A, A, B, A, A, A, A, B,
            B, A, B, B, A, A, A, A, A, A, A, A, A, A, A, B, B, A, B,
            B, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, B,
            B, A, A, A, A, B, B, B, A, B, A, B, B, B, A, A, A, A, B,
            B, A, A, A, A, B, A, A, A, A, A, A, A, B, A, A, A, A, B,
            B, B, B, B, B, B, B, B, B, A, B, B, B, B, B, B, B, B, B,*/
    B,B,B,B,A,B,B,B,B,B,B,B,B,B,A,B,B,B,B,
    B,A,O,A,O,A,O,A,O,B,O,A,O,A,O,A,O,A,B,
    B,O,B,B,A,B,A,B,A,O,A,B,A,B,A,B,B,O,B,
    B,A,B,B,O,B,O,B,B,B,B,B,O,B,O,B,B,A,B,
    B,O,A,O,A,O,A,O,A,B,A,O,A,O,A,O,A,O,B,
    B,B,B,B,O,B,B,B,O,B,O,B,B,B,O,B,B,B,B,
    B,O,A,O,A,B,A,O,A,O,A,O,A,B,A,O,A,O,B,
    B,B,B,B,O,B,O,B,S,S,S,B,O,B,O,B,B,B,B,
    A,O,A,O,A,O,A,B,S,S,S,B,A,O,A,O,A,O,A,
    B,B,B,B,O,B,O,B,B,B,B,B,O,B,O,B,B,B,B,
    B,O,A,O,A,B,A,O,A,O,A,O,A,B,A,O,A,O,B,
    B,B,B,B,O,B,O,B,B,B,B,B,O,B,O,B,B,B,B,
    B,O,A,O,A,O,A,O,A,B,A,O,A,O,A,O,A,O,B,
    B,A,B,B,O,B,B,B,O,B,O,B,B,B,O,B,B,A,B,
    B,O,A,O,A,O,A,O,A,O,A,O,A,O,A,O,A,O,B,
    B,B,O,B,O,B,O,B,B,B,B,B,O,B,O,B,O,B,B,
    B,B,A,B,A,B,A,B,B,B,B,B,A,B,A,B,A,B,B,
    B,A,O,A,O,A,O,A,O,A,O,A,O,A,O,A,O,A,B,
    B,B,B,B,A,B,B,B,B,B,B,B,B,B,A,B,B,B,B

};
#undef N
#undef A
#undef B
#undef O
#undef S
Stage_1::Stage_1()
{
    BeansNum=0;

    for (int i = 0; i < MAPLENGTH; i++) {
        for (int j = 0; j < MAPLENGTH; j++) {
            WallData[i][j] = BeanData[i][j] = RoadData[i][j]=0;

            switch (initData[i][j])
            {
            case -1:
                RoadData[i][j]=-1;
                break;
            case 0:
                RoadData[i][j]=1;
                break;
            case 1:
                WallData[i][j]=true;
                break;
            case 2:
                BeanData[i][j] = 1;
                RoadData[i][j]=1;
                ++BeansNum;
                break;
            }
        }
    }

    PR_Bean=6;

}

#define N 3
#define A 2
#define B 1
#define O 0
#define S -1
const int Stage_2::initData[MAPLENGTH][MAPLENGTH] = {
    B, B, B, B, B, B, B, B, B, A, B, B, B, A, B, B, B, B, B,
    A, A, A, A, A, A, A, B, A, A, B, A, A, A, B, A, B, A, A,
    B, A, A, A, B, A, A, B, A, A, B, A, B, A, B, A, B, A, B,
    B, B, B, A, B, A, A, B, B, A, B, A, B, A, B, A, B, B, B,
    B, A, A, A, A, A, A, A, B, A, A, A, A, A, B, A, A, A, B,
    B, A, A, B, A, A, A, B, B, B, B, B, A, A, A, A, A, A, B,
    B, A, A, B, A, A, A, A, A, A, A, A, A, A, A, B, A, A, B,
    B, A, A, B, A, B, A, B, S, S, S, B, A, A, A, B, A, A, B,
    B, A, A, B, A, B, A, B, S, S, S, B, A, B, A, B, A, A, B,
    A, A, A, B, A, B, A, B, B, B, B, B, A, B, A, B, A, A, A,
    B, A, A, B, A, B, A, A, A, O, A, A, A, B, A, A, A, A, B,
    B, A, A, B, A, A, A, A, A, A, A, A, A, B, A, A, A, A, B,
    B, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, B,
    B, A, A, A, B, B, B, B, B, B, B, A, A, A, A, A, A, A, B,
    B, A, A, A, A, A, A, A, A, A, A, A, A, B, A, A, A, A, B,
    B, B, B, B, B, A, A, A, A, B, B, B, A, B, A, A, A, A, B,
    B, A, A, A, B, B, B, A, A, A, A, B, A, B, B, B, A, A, B,
    A, A, A, A, B, A, A, A, A, A, A, B, A, A, A, B, A, A, A,
    B, B, B, B, B, B, B, B, B, A, B, B, B, A, B, B, B, B, B,
};
#undef N
#undef A
#undef B
#undef O
#undef S
Stage_2::Stage_2()
{
    BeansNum=0;

    for (int i = 0; i < MAPLENGTH; i++) {
        for (int j = 0; j < MAPLENGTH; j++) {
            WallData[i][j] = BeanData[i][j] = RoadData[i][j]=0;

            switch (initData[i][j])
            {
            case -1:
                RoadData[i][j]=-1;
                break;
            case 0:
                RoadData[i][j]=1;
                break;
            case 1:
                WallData[i][j]=true;
                break;
            case 2:
                BeanData[i][j] = 1;
                RoadData[i][j]=1;
                ++BeansNum;
                break;
            }
        }
    }
    PR_Bean=5;

}

#define N 3
#define A 2
#define B 1
#define O 0
#define S -1

const int Stage_3::initData[MAPLENGTH][MAPLENGTH] = {
    B, B, B, B, B, B, B, B, B, A, B, B, B, B, B, B, B, B, B,
    A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    B, A, A, B, A, A, B, B, B, B, B, B, B, A, A, A, B, A, B,
    B, A, B, B, A, A, A, A, A, A, A, A, B, A, A, A, B, A, B,
    B, A, B, A, A, A, B, B, B, B, B, B, B, A, A, A, B, A, B,
    B, A, B, A, B, B, B, A, A, A, A, A, B, B, B, A, B, A, B,
    B, A, A, A, B, A, B, A, A, A, A, A, A, A, A, A, B, A, B,
    B, A, B, A, B, A, A, A, A, A, A, A, A, B, A, A, B, A, B,
    B, A, B, A, B, B, A, A, B, A, B, A, A, B, A, A, B, A, B,
    B, A, A, A, A, B, A, A, B, B, B, A, A, B, A, A, B, A, B,
    B, A, B, A, A, B, A, A, A, A, A, A, A, B, A, A, A, A, B,
    B, A, B, A, A, B, A, A, A, A, A, A, B, B, B, A, B, A, B,
    B, A, B, A, A, B, A, B, B, B, B, B, B, A, B, A, B, A, B,
    B, A, B, A, A, B, A, A, A, A, A, A, A, A, B, A, B, A, B,
    B, A, B, B, A, B, B, B, B, B, B, A, B, A, B, A, B, A, B,
    B, A, A, A, A, B, A, A, A, A, A, A, B, A, B, A, B, A, B,
    B, B, B, B, B, B, A, A, B, B, B, A, B, A, B, A, B, A, B,
    A, A, A, A, A, A, A, A, B, A, A, A, A, A, B, A, A, A, A,
    B, B, B, B, B, B, B, B, B, A, B, B, B, B, B, B, B, B, B,
};

#undef N
#undef A
#undef B
#undef O
#undef S
Stage_3::Stage_3()
{
    BeansNum=0;

    for (int i = 0; i < MAPLENGTH; i++) {
        for (int j = 0; j < MAPLENGTH; j++) {
            WallData[i][j] = BeanData[i][j] = RoadData[i][j]=0;

            switch (initData[i][j])
            {
            case -1:
                RoadData[i][j]=-1;
                break;
            case 0:
                RoadData[i][j]=1;
                break;
            case 1:
                WallData[i][j]=true;
                break;
            case 2:
                BeanData[i][j] = 1;
                RoadData[i][j]=1;
                ++BeansNum;
                break;
            }
        }
    }
    PR_Bean=4;

}
