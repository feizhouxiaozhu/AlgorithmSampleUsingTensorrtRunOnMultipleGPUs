





#include "ysharedframe.h"

YSharedFrame::YSharedFrame(YDataType data_type, int device, QObject *parent)
    : YImage(data_type, device), YSharedData(parent)
{

}

YSharedFrame::~YSharedFrame()
{

}
