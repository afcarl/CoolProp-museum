
#ifndef FLUIDLIBRARY_H
#define FLUIDLIBRARY_H

#include "CoolPropFluid.h"

#include "rapidjson/rapidjson_include.h"

#include <map>
#include <algorithm>



namespace CoolProp{

/// A container for the fluid parameters for the CoolProp fluids
/**
This container holds copies of all of the fluid instances for the fluids that are loaded in CoolProp. 
New fluids can be added by passing in a rapidjson::Value instance to the add_one function, or 
a rapidjson array of fluids to the add_many function.
*/
class JSONFluidLibrary
{
    /// Map from CAS code to JSON instance.  For pseudo-pure fluids, use name in place of CAS code since no CASE number is defined for mixtures
    std::map<std::size_t, CoolPropFluid> fluid_map;

    std::map<std::string, std::size_t> string_to_index_map;
    bool _is_empty;
protected:

    /// Parse the contributions to the residual Helmholtz energy
    void parse_alphar(rapidjson::Value &alphar, EquationOfState &EOS)
    {	
        for (rapidjson::Value::ValueIterator itr = alphar.Begin(); itr != alphar.End(); ++itr)
        {	
            // A reference for code cleanness
            rapidjson::Value &contribution = *itr;

            // Get the type (required!)
            std::string type = contribution["type"].GetString();

            if (!type.compare("ResidualHelmholtzPower"))
            {
                std::vector<long double> n = cpjson::get_long_double_array(contribution["n"]);
                std::vector<long double> d = cpjson::get_long_double_array(contribution["d"]);
                std::vector<long double> t = cpjson::get_long_double_array(contribution["t"]);
                std::vector<long double> l = cpjson::get_long_double_array(contribution["l"]);
                EOS.alphar.Power = ResidualHelmholtzPower(n,d,t,l);
            }
            else if (!type.compare("ResidualHelmholtzGaussian"))
            {
                std::vector<long double> n = cpjson::get_long_double_array(contribution["n"]);
                std::vector<long double> d = cpjson::get_long_double_array(contribution["d"]);
                std::vector<long double> t = cpjson::get_long_double_array(contribution["t"]);
                std::vector<long double> eta = cpjson::get_long_double_array(contribution["eta"]);
                std::vector<long double> epsilon = cpjson::get_long_double_array(contribution["epsilon"]);
                std::vector<long double> beta = cpjson::get_long_double_array(contribution["beta"]);
                std::vector<long double> gamma = cpjson::get_long_double_array(contribution["gamma"]);
                EOS.alphar.Gaussian = ResidualHelmholtzGaussian(n,d,t,eta,epsilon,beta,gamma);
            }
            else if (!type.compare("ResidualHelmholtzNonAnalytic"))
            {
                std::vector<long double> n = cpjson::get_long_double_array(contribution["n"]);
                std::vector<long double> a = cpjson::get_long_double_array(contribution["a"]);
                std::vector<long double> b = cpjson::get_long_double_array(contribution["b"]);
                std::vector<long double> beta = cpjson::get_long_double_array(contribution["beta"]);
                std::vector<long double> A = cpjson::get_long_double_array(contribution["A"]);
                std::vector<long double> B = cpjson::get_long_double_array(contribution["B"]);
                std::vector<long double> C = cpjson::get_long_double_array(contribution["C"]);
                std::vector<long double> D = cpjson::get_long_double_array(contribution["D"]);
                EOS.alphar.NonAnalytic = ResidualHelmholtzNonAnalytic(n,a,b,beta,A,B,C,D);
            }
            else if (!type.compare("ResidualHelmholtzLemmon2005"))
            {
                std::vector<long double> n = cpjson::get_long_double_array(contribution["n"]);
                std::vector<long double> d = cpjson::get_long_double_array(contribution["d"]);
                std::vector<long double> t = cpjson::get_long_double_array(contribution["t"]);
                std::vector<long double> l = cpjson::get_long_double_array(contribution["l"]);
                std::vector<long double> m = cpjson::get_long_double_array(contribution["m"]);
                EOS.alphar.Lemmon2005 = ResidualHelmholtzLemmon2005(n,d,t,l,m);
            }
            else if (!type.compare("ResidualHelmholtzExponential"))
            {
                std::vector<long double> n = cpjson::get_long_double_array(contribution["n"]);
                std::vector<long double> d = cpjson::get_long_double_array(contribution["d"]);
                std::vector<long double> t = cpjson::get_long_double_array(contribution["t"]);
                std::vector<long double> g = cpjson::get_long_double_array(contribution["g"]);
                std::vector<long double> l = cpjson::get_long_double_array(contribution["l"]);
                EOS.alphar.Exponential = ResidualHelmholtzExponential(n,d,t,g,l);
            }
            else if (!type.compare("ResidualHelmholtzAssociating"))
            {
                long double a = cpjson::get_double(contribution,"a");
                long double m = cpjson::get_double(contribution,"m");
                long double epsilonbar = cpjson::get_double(contribution,"epsilonbar");
                long double vbarn = cpjson::get_double(contribution,"vbarn");
                long double kappabar = cpjson::get_double(contribution,"kappabar");
                EOS.alphar.SAFT = ResidualHelmholtzSAFTAssociating(a,m,epsilonbar,vbarn,kappabar);
            }
            else
            {
                throw ValueError(format("Unsupported Residual helmholtz type: %s",type.c_str()));
            }
        }
    };

    /// Parse the contributions to the ideal-gas Helmholtz energy
    void parse_alpha0(rapidjson::Value &alpha0, EquationOfState &EOS)
    {
        if (!alpha0.IsArray()){throw ValueError();}
        for (rapidjson::Value::ValueIterator itr = alpha0.Begin(); itr != alpha0.End(); ++itr)
        {	
            // A reference for code cleanness
            rapidjson::Value &contribution = *itr;

            // Get the type (required!)
            std::string type = contribution["type"].GetString();

            if (!type.compare("IdealGasHelmholtzLead"))
            {
                long double a1 = cpjson::get_double(contribution,"a1");
                long double a2 = cpjson::get_double(contribution,"a2");
                EOS.alpha0.Lead = IdealHelmholtzLead(a1, a2);
            }
            else if (!type.compare("IdealGasHelmholtzPower"))
            {
                std::vector<long double> n = cpjson::get_long_double_array(contribution["n"]);
                std::vector<long double> t = cpjson::get_long_double_array(contribution["t"]);
                EOS.alpha0.Power = IdealHelmholtzPower(n, t);
            }
            else if (!type.compare("IdealGasHelmholtzLogTau"))
            {
                long double a = cpjson::get_double(contribution,"a");
                EOS.alpha0.LogTau = IdealHelmholtzLogTau(a);
            }
            else if (!type.compare("IdealGasHelmholtzPlanckEinstein"))
            {
                std::vector<long double> n = cpjson::get_long_double_array(contribution["n"]);
                std::vector<long double> t = cpjson::get_long_double_array(contribution["t"]);
                EOS.alpha0.PlanckEinstein = IdealHelmholtzPlanckEinstein(n, t);
            }
            else if (!type.compare("IdealGasHelmholtzPlanckEinstein2"))
            {
                std::vector<long double> n = cpjson::get_long_double_array(contribution["n"]);
                std::vector<long double> t = cpjson::get_long_double_array(contribution["t"]);
                std::vector<long double> c = cpjson::get_long_double_array(contribution["c"]);
                EOS.alpha0.PlanckEinstein2 = IdealHelmholtzPlanckEinstein2(n, t, c);
            }
            else if (!type.compare("IdealGasHelmholtzEnthalpyEntropyOffset"))
            {
                long double a1 = cpjson::get_double(contribution, "a1");
                long double a2 = cpjson::get_double(contribution, "a2");
                EOS.alpha0.EnthalpyEntropyOffset = IdealHelmholtzEnthalpyEntropyOffset(a1, a2);
            }
            else if (!type.compare("IdealGasHelmholtzCP0Constant"))
            {
                long double cp_over_R = cpjson::get_double(contribution, "cp_over_R");
                long double Tc = cpjson::get_double(contribution, "Tc");
                long double T0 = cpjson::get_double(contribution, "T0");
                EOS.alpha0.CP0Constant = IdealHelmholtzCP0Constant(cp_over_R, Tc, T0);
            }
            else if (!type.compare("IdealGasHelmholtzCP0PolyT"))
            {
                std::vector<long double> c = cpjson::get_long_double_array(contribution["c"]);
                std::vector<long double> t = cpjson::get_long_double_array(contribution["t"]);
                long double Tc = cpjson::get_double(contribution, "Tc");
                long double T0 = cpjson::get_double(contribution, "T0");
                EOS.alpha0.CP0PolyT = IdealHelmholtzCP0PolyT(c, t, Tc, T0);
            }
            else if (!type.compare("IdealGasHelmholtzCP0AlyLee"))
            {
                std::vector<long double> c = cpjson::get_long_double_array(contribution["c"]);
                long double Tc = cpjson::get_double(contribution, "Tc");
                long double T0 = cpjson::get_double(contribution, "T0");
                EOS.alpha0.CP0AlyLee = IdealHelmholtzCP0AlyLee(c, Tc, T0);
            }
            else
            {
                std::cout << format("Unsupported ideal-gas Helmholtz type: %s\n",type.c_str());
                //throw ValueError(format("Unsupported ideal-gas Helmholtz type: %s",type.c_str()));
            }
        }
    };

    /// Parse the Equation of state JSON entry
    void parse_EOS(rapidjson::Value &EOS_json, CoolPropFluid &fluid)
    {
        EquationOfState E;
        fluid.EOSVector.push_back(E);

        EquationOfState &EOS = fluid.EOSVector.at(fluid.EOSVector.size()-1);

        // Universal gas constant [J/mol/K]
        EOS.R_u = cpjson::get_double(EOS_json,"gas_constant");
        EOS.molar_mass = cpjson::get_double(EOS_json,"molar_mass");
        EOS.accentric = cpjson::get_double(EOS_json,"accentric");
        EOS.Ttriple = cpjson::get_double(EOS_json, "Ttriple");
        EOS.ptriple = cpjson::get_double(EOS_json, "ptriple");
        EOS.pseudo_pure = cpjson::get_bool(EOS_json, "pseudo_pure");

        rapidjson::Value &reducing_state = EOS_json["reducing_state"];
        
        // Reducing state
        EOS.reduce.T = cpjson::get_double(reducing_state,"T");
        EOS.reduce.rhomolar = cpjson::get_double(reducing_state,"rhomolar");
        EOS.reduce.p = cpjson::get_double(reducing_state,"p");
        
        parse_alphar(EOS_json["alphar"], EOS);
        parse_alpha0(EOS_json["alpha0"], EOS);
        
        // Validate the equation of state that was just created
        EOS.validate();
        
    }

    /// Parse the list of possible equations of state
    void parse_EOS_listing(rapidjson::Value &EOS_array, CoolPropFluid & fluid)
    {
        for (rapidjson::Value::ValueIterator itr = EOS_array.Begin(); itr != EOS_array.End(); ++itr)
        {	
            parse_EOS(*itr,fluid);
        }

        // Set the EOS pointer to the first EOS
        fluid.pEOS = &(fluid.EOSVector[0]);
    };

    /// Parse the reducing state for the given EOS
    void parse_reducing_state(rapidjson::Value &alphar)
    {
    };

    /// Parse the critical state for the given EOS
    void parse_crit_state(rapidjson::Value &alphar)
    {
    };

    /// Parse the critical state for the given EOS
    void parse_ancillaries(rapidjson::Value &ancillaries, CoolPropFluid & fluid)
    {
        if (!ancillaries.HasMember("pL") || !ancillaries.HasMember("pV") || !ancillaries.HasMember("rhoL") || !ancillaries.HasMember("rhoV")){throw ValueError("Ancillary curves are missing");};
        fluid.ancillaries.pL = AncillaryFunction(ancillaries["pL"]);
        fluid.ancillaries.pV = AncillaryFunction(ancillaries["pV"]);
        fluid.ancillaries.rhoL = AncillaryFunction(ancillaries["rhoL"]);
        fluid.ancillaries.rhoV = AncillaryFunction(ancillaries["rhoV"]);
    };

    /// Validate the fluid file that was just constructed
    void validate(CoolPropFluid & fluid)
    {
        assert(fluid.EOSVector.size() > 0);
        assert(fluid.CAS.length() > 0);
        assert(fluid.name.length() > 0);
    }
public:
    
    // Default constructor;
    JSONFluidLibrary(){
        _is_empty = true;
    };
    bool is_empty(void){ return _is_empty;};

    /// Add all the fluid entries in the rapidjson::Value instance passed in
    void add_many(rapidjson::Value &listing)
    {
        for (rapidjson::Value::ValueIterator itr = listing.Begin(); itr != listing.End(); ++itr)
        {	
            add_one(*itr);
        }
    };
    void add_one(rapidjson::Value &fluid_json)
    {
        _is_empty = false;
        
        // Get the next index for this fluid
        std::size_t index = fluid_map.size();

        // Add index->fluid mapping
        fluid_map[index] = CoolPropFluid();

        // Create an instance of the fluid
        CoolPropFluid &fluid = fluid_map[index];

        // Fluid name
        fluid.name = fluid_json["NAME"].GetString();
        // CAS number
        fluid.CAS = fluid_json["CAS"].GetString();

        // Aliases
        fluid.aliases = cpjson::get_string_array(fluid_json["ALIASES"]);
        
        // EOS
        parse_EOS_listing(fluid_json["EOS"],fluid);

        // Validate the fluid
        validate(fluid);

        // Ancillaries
        if (!fluid_json.HasMember("ANCILLARIES")){throw ValueError(format("Ancillary curves are missing for fluid [%s]",fluid.name.c_str()));};
        parse_ancillaries(fluid_json["ANCILLARIES"],fluid);
        
        // If the fluid is ok...

        // Add CAS->index mapping
        string_to_index_map[fluid.CAS] = index;

        // Add name->index mapping
        string_to_index_map[fluid.name] = index;
        
        // Add the aliases
        for (std::size_t i = 0; i < fluid.aliases.size(); ++i)
        {
            string_to_index_map[fluid.aliases[i]] = index;
        }

    };
    /// Get a CoolPropFluid instance stored in this library
    /**
    @param key Either a CAS number or the name (CAS number should be preferred)
    */
    CoolPropFluid& get(std::string key)
    {
        std::map<std::string, std::size_t>::iterator it;
        // Try to find it
        it = string_to_index_map.find(key);
        // If it is found
        if (it != string_to_index_map.end()){
            return get(it->second);
        }
        else{
            throw ValueError(format("key [%s] was not found in string_to_index_map in JSONFluidLibrary",key.c_str()));
        }
    };
    /// Get a CoolPropFluid instance stored in this library
    /**
    @param key The index of the fluid in the map
    */
    CoolPropFluid& get(std::size_t key)
    {
        std::map<std::size_t,CoolPropFluid>::iterator it;
        // Try to find it
        it = fluid_map.find(key);
        // If it is found
        if (it != fluid_map.end()){
            return it->second;
        }
        else{
            throw ValueError(format("key [%d] was not found in JSONFluidLibrary",key));
        }
    };
};

/// Get a reference to the library instance
JSONFluidLibrary & get_library(void);

} /* namespace CoolProp */
#endif