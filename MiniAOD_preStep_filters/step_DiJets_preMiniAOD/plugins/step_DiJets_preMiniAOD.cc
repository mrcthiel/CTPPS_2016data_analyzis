#include "MiniAOD_preStep_filters/Analysis_Tools.h"

class step_DiJets_preMiniAOD: public edm::stream::EDFilter<> {
	public:
		explicit step_DiJets_preMiniAOD(const edm::ParameterSet&);


	private:
		virtual bool filter(edm::Event&, const edm::EventSetup&) override;

		edm::EDGetTokenT<std::vector<reco::PFJet>> jetsToken;
		edm::Handle<std::vector<reco::PFJet>> jets;
		double eta_cut, pt_jet1, pt_jet2, pt_jet3, pt_jet4, pt_jet_others;
		size_t  N_jet_min, N_jet_max;
};


step_DiJets_preMiniAOD::step_DiJets_preMiniAOD(const edm::ParameterSet& iConfig):
	jetsToken (consumes<std::vector<reco::PFJet>>(iConfig.getParameter<edm::InputTag>("jets")))
	, eta_cut (iConfig.getParameter<double>("eta_cut"))
	, pt_jet1 (iConfig.getParameter<double>("pt_jet1"))
	, pt_jet2 (iConfig.getParameter<double>("pt_jet2"))
	, pt_jet3 (iConfig.getParameter<double>("pt_jet3"))
	, pt_jet4 (iConfig.getParameter<double>("pt_jet4"))
	, pt_jet_others (iConfig.getParameter<double>("pt_jet_others"))
	, N_jet_min (iConfig.getParameter<double>("N_jet_min"))
	, N_jet_max (iConfig.getParameter<double>("N_jet_max")){
	}

bool step_DiJets_preMiniAOD::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;
	using namespace std;

	iEvent.getByToken(jetsToken, jets);

	bool tightLepVetoJetID;
	double NHF, NEMF, CHF, MUF, CEMF, NumConst, CHM;

	vector<double> pt_jet = {pt_jet1,pt_jet2,pt_jet3,pt_jet4};
	if (jets->size() > 4) {
		for (size_t ii = 0; ii < (jets->size()-4); ii++) {pt_jet.push_back(pt_jet_others);}
	}

	vector<size_t> selected_jets;
	for(size_t i=0; i<jets->size(); i++) {
		if(jets->at(i).pt()<pt_jet[i] || abs(jets->at(i).eta())>eta_cut) continue;
		NHF  = jets->at(i).neutralHadronEnergyFraction();
		NEMF = jets->at(i).neutralEmEnergyFraction();
		CHF  = jets->at(i).chargedHadronEnergyFraction();
		MUF  = jets->at(i).muonEnergyFraction();
		CEMF = jets->at(i).chargedEmEnergyFraction();
		NumConst = jets->at(i).chargedMultiplicity()+jets->at(i).neutralMultiplicity();
		//	   NumNeutralParticles = jets->at(jets_selected->at(i)).neutralMultiplicity();
		CHM      = jets->at(i).chargedMultiplicity(); 
		tightLepVetoJetID = (NHF<0.90 && NEMF<0.90 && NumConst>1 && MUF<0.8 && CHF>0 && CHM>0 && CEMF<0.90);
		if (tightLepVetoJetID) {
			selected_jets.push_back(i);
		}
	}

	if (selected_jets.size() < N_jet_min && selected_jets.size() > N_jet_max) {return false;}   else {
		return true;
	}
}

//define this as a plug-in
DEFINE_FWK_MODULE(step_DiJets_preMiniAOD);
