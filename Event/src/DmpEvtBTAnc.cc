#include "DmpEvtBTAnc.h"
#include <cmath>
//#include <string.h>
ClassImp(DmpEvtAMSCluster);

//-------------------------------------------------------------------
DmpEvtAMSCluster::DmpEvtAMSCluster(){
  Reset();
}

//-------------------------------------------------------------------
DmpEvtAMSCluster::DmpEvtAMSCluster(const DmpEvtAMSCluster &orig):TObject(orig){
  address=orig.address;
  length=orig.length;
  Signal=orig.Signal;
  Noise=orig.Noise;
  Status=orig.Status;
  ladder=orig.ladder;
  side=orig.side;
  bad=orig.bad;
  //golden=orig.golden;
  snratio =orig.snratio;
  CNstatus=orig.CNstatus;
  powbits = orig.powbits;
}

//-------------------------------------------------------------------
void DmpEvtAMSCluster::Build(int lad, int sid, int add, int len, float* sig, float* noi, int* stat, int Sig2NoiRatio, int CNStatus, int PowBits, bool badin){
  ladder=lad;
  side=sid;
  address=add;
  static int MAXLENGHT = 50;
  if(len>MAXLENGHT) length=MAXLENGHT;
  else length=len;
  for (int ii=0;ii<length;ii++){
    Signal[ii]=sig[ii];
    Noise[ii]=noi[ii];
    Status[ii]=stat[ii];
  }
  bad=badin;
  snratio=Sig2NoiRatio;
  CNstatus=CNStatus;
  powbits=PowBits;
}

//-------------------------------------------------------------------
void DmpEvtAMSCluster::LoadFrom(const DmpEvtAMSCluster *&r){
  ladder = r->ladder;
  side = r->side;
  address = r->address;
  length = r->length;
  Signal = r->Signal;
  Noise = r->Noise;
  Status = r->Status;
  bad = r->bad;
  //golden = r->golden;
  snratio =r->snratio;
  CNstatus =r->CNstatus;
  powbits = r->powbits;
}

//-------------------------------------------------------------------
void DmpEvtAMSCluster::Reset(){
  ladder=-1;
  side=-1;
  address=0;
  length=0;
  Signal.clear();
  Noise.clear();
  Status.clear();
  bad=false;
  //golden = 0;
}

//-------------------------------------------------------------------
int DmpEvtAMSCluster::GetSeed()const{
  float max=-1000;
  int seed=-1;
  for (int ii=0;ii<length;ii++){
    if (Signal[ii]/Noise[ii]>=max&&Status[ii]==0
	&&!(ladder==5&&(ii+address)==380)
	&&!(ladder==5&&(ii+address)==381)
	){
      max=Signal[ii]/Noise[ii];
      seed=ii;
    }
  }
  return seed;
}

//-------------------------------------------------------------------
float DmpEvtAMSCluster::GetSeedSN()const{
  int seed = GetSeed();
  return GetCSignal(seed)/Noise[seed];
}

//-------------------------------------------------------------------
float DmpEvtAMSCluster::GetTotNoise()const{
  int se=GetSeed();
  int se_r=se+1;
  int se_l=se-1;
  float val_l=0;
  float val_r=0;
  if (se_l>=0)     val_l = Noise[se_l];
  else val_l=0;
  if (se_r<length) val_r = Noise[se_r];
  else val_r=0;
  if(val_l>val_r) return sqrt(val_l*val_l+Noise[se]*Noise[se]);
  else return sqrt(val_r*val_r+Noise[se]*Noise[se]);
}

//-------------------------------------------------------------------
float DmpEvtAMSCluster::GetCoG()const{
  int se=GetSeed();
  float ee=GetEtaRaw();
  if(ee<0) return address+se;
  else return address+ee;
}

//-------------------------------------------------------------------
float DmpEvtAMSCluster::GetTotSig()const{
  int se=GetSeed();
  int se_r=se+1;
  int se_l=se-1;
  float val_l=0;
  float val_r=0;
  if (se_l>=0)     val_l = GetCSignal(se_l);
  else val_l=0;
  if (se_r<length) val_r = GetCSignal(se_r);
  else val_r=0;
  if(val_l>val_r) return val_l+GetCSignal(se);
  else return val_r+GetCSignal(se);
}

//-------------------------------------------------------------------
float DmpEvtAMSCluster::GetEta()const{
  float ee=GetEtaRaw();
  if(ee<0) return -3;
  else return ee-trunc(ee);
}

//-------------------------------------------------------------------
float DmpEvtAMSCluster::GetEtaRaw()const{
  float ee;
  int se=GetSeed();
  int se_r=se+1;
  int se_l=se-1;
  float val_l=0;
  float val_r=0;
  float val_seed=GetCSignal(se);
  if (se_l>=0)     val_l = GetCSignal(se_l);
  if (se_r<length) val_r = GetCSignal(se_r);

  if(val_l>val_r && val_l/Noise[se_l]>0)      ee=(val_seed*se+val_l*se_l)/(val_l+val_seed);
  else if(val_r>val_l && val_r/Noise[se_r]>0) ee=(val_r*se_r+val_seed*se)/(val_r+val_seed);
  else return -3;

  return ee;
}

//-------------------------------------------------------------------
float DmpEvtAMSCluster::GetCenterPosition()const{
  float pos =0.;
  for(short i=0;i<length;++i){
    pos += (address+i) * Signal[i];
  }
  return pos / GetTotSig();
}

//-------------------------------------------------------------------
void DmpEvtAMSCluster::Print()const{
  printf(" ladd: %d  side: %d add: %d len: %d bad: %d \n",
         ladder,side,address,length,bad);
  printf(" Seed: %d SeedAddress: %d Seed:val %f SeedSN: %f \n",GetSeed(),GetSeedAdd(),GetSeedVal(),GetSeedSN());
  printf(" Strip:       Signal:     Noise:       Status: \n");
  for(int ii=0;ii<length;ii++){
    printf("    %d      %f       %f       %d \n",
	   ii, GetCSignal(ii), Noise[ii], Status[ii]);
  }
  return;
}

//-------------------------------------------------------------------
int DmpEvtAMSCluster::GoldRegion()const{
  int lowgold[6][2]={{280,720},{50,650},{150,660},{160,660},{150,860},{220,660}};
  int  upgold[6][2]={{340,830},{150,850},{400,710},{460,710},{450,920},{480,710}};

  if(GetCoG()>lowgold[ladder][side]&&
     GetCoG()<=upgold[ladder][side]) return 1;
  else return 0;
}

//-------------------------------------------------------------------
int DmpEvtAMSCluster::GetLength(float val)const{
  int se=GetSeed();
  int myle=1;
  for (int ii=se-1;ii>=0;ii--)
    if(GetCSignal(ii)/Noise[ii]>val) myle++;
    else break;
  for (int ii=se+1;ii<length;ii++)
    if(GetCSignal(ii)/Noise[ii]>val) myle++;
    else break;
  return myle;
}

//-------------------------------------------------------------------
float DmpEvtAMSCluster::GetCSignal(int aa)const{
 //  int stadd=address+aa;
//   int vanum=stadd/64;
//   float c1= Signal[aa];
//   float corr=0.;
//   if(aa>0){
//     int stadd2=address+aa-1;
//     int vanum2=stadd2/64;
//     float c2= Signal[aa-1];
//     corr=c2*0.;
//   }
//   if (side==1) return c1-corr;
//   else return c1;
  return  Signal[aa];
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
ClassImp(DmpEvtBTAnc);

//-------------------------------------------------------------------
DmpEvtBTAnc::DmpEvtBTAnc():fAMSClusters(0){
  fAMSClusters = new TClonesArray("DmpEvtAMSCluster",10);
  Reset();
  fAMSClusters->SetOwner();
}

//-------------------------------------------------------------------
DmpEvtBTAnc::~DmpEvtBTAnc(){
  fAMSClusters->Delete();
  delete fAMSClusters;
}

//-------------------------------------------------------------------
void DmpEvtBTAnc::LoadFrom(const DmpEvtBTAnc *&r){
  fAMSClusters = r->fAMSClusters;
  fAdcCherenkov1 = r->fAdcCherenkov1;
  fAdcCherenkov2 = r->fAdcCherenkov2;
  fAdcPbGlass = r->fAdcPbGlass;
  fAdcSc1 = r->fAdcSc1;
  fAdcSc2 = r->fAdcSc2;
  fAdcSc3 = r->fAdcSc3;
  fAdcSc4 = r->fAdcSc4;
  fAdcSd1 = r->fAdcSd1;
  fAdcSd2 = r->fAdcSd2;
}

//-------------------------------------------------------------------
void DmpEvtBTAnc::Reset(){
  fAMSClusters->Delete();
  fAdcCherenkov1 = 0;
  fAdcCherenkov2 = 0;
  fAdcPbGlass = 0;
  fAdcSc1 = 0;
  fAdcSc2 = 0;
  fAdcSc3 = 0;
  fAdcSc4 = 0;
  fAdcSd1 = 0;
  fAdcSd2 = 0;
}

