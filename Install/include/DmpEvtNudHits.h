
#ifndef DmpEvtNudHits_H
#define DmpEvtNudHits_H

#include "TObject.h"
class DmpEvtNudHits : public TObject{
        /*
         * * DmpEvtNudHits
         * *
         * */
        public:
                DmpEvtNudHits();
                ~DmpEvtNudHits();
                void Reset();
        public:
                /*For Eventclass, if changed, add to Reset() */
                std::vector<short> fChannelID;
                std::vector<double> fEnergy;
                ClassDef(DmpEvtNudHits,1)
};
#endif
