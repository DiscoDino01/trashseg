#ifndef __OTHERBOARD_
#define __OTHERBOARD_
#define sizeofUINT8 sizeof(__UINT8_TYPE__)
#define sizeofUINT8_bit (sizeofUINT8 * 8)

typedef __UINT8_TYPE__ uint8;

/*extern*/ namespace OtherBoard {

    extern void send_data(int, uint8)/*{
        
    }*/;


};


#endif