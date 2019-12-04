






#ifndef YALGORITHM_H
#define YALGORITHM_H

#include "utils/ylaborcontractor.h"

class YAlgorithmLaborContractor : public YLaborContractor
{
public:
    YAlgorithmLaborContractor(int worker_count, QObject *parent);
    virtual ~YAlgorithmLaborContractor() {}

};

#endif // YALGORITHM_H
