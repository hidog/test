#ifndef __MAXPRODBP_H__
#define __MAXPRODBP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mrf.h"
#include <QImage>
#include "LinkedBlockList.h"
#include "regions-new.h"

#define FLOATTYPE float
#define FloatType float

class MaxProdBP : public MRF{
public:

	void	save_ans();

    MaxProdBP(int width, int height, int nLabels, EnergyFunction *eng);
    MaxProdBP(int nPixels, int nLabels,EnergyFunction *eng);
    ~MaxProdBP();
    void setNeighbors(int pix1, int pix2, CostVal weight);
    Label getLabel(int pixel){return(m_answer[pixel]);};
    void setLabel(int pixel,Label label){m_answer[pixel] = label;};
    Label* getAnswerPtr(){return(m_answer);};
    void clearAnswer();
    void setParameters(int /*numParam*/, void * /*param*/){printf("No optional parameters to set");}
    EnergyVal smoothnessEnergy();
    EnergyVal dataEnergy();
    int getWidth();
    int getHeight();
    FLOATTYPE *getScratchMatrix();
    int getNLabels();
    bool varWeights();
    FLOATTYPE getExpV(int i);
    FLOATTYPE *getExpV();

    CostVal getHorizWeight(int r, int c);
    CostVal getVertWeight(int r, int c);
protected:
    void setData(DataCostFn dcost); 
    void setData(CostVal* data);    
    void setSmoothness(SmoothCostGeneralFn cost);
    void setSmoothness(CostVal* V);
    void setSmoothness(int smoothExp,CostVal smoothMax, CostVal lambda);
    void setCues(CostVal* hCue, CostVal* vCue); 
    void initializeAlg();
    void optimizeAlg(int nIterations);

private:
    Label *m_answer;
    CostVal *m_V;
    CostVal *m_D;
    CostVal *m_horizWeights;
    CostVal *m_vertWeights;
    FLOATTYPE m_exp_scale;
    DataCostFn m_dataFn;
    SmoothCostGeneralFn m_smoothFn;
    bool m_needToFreeV;
    FLOATTYPE *m_scratchMatrix;
    FLOATTYPE *m_ExpData;
    FLOATTYPE *m_message_chunk;
    OneNodeCluster *nodeArray;
    typedef struct NeighborStruct {
	int     to_node;
	CostVal weight;
    } Neighbor;

    LinkedBlockList *m_neighbors;
};


#endif /* __MAXPRODBP_H__ */


