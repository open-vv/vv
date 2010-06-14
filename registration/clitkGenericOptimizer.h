/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/

#ifndef __clitkGenericOptimizer_h
#define __clitkGenericOptimizer_h

//clitk include
#include "clitkLBFGSBOptimizer.h"

//itk include
#include "itkAmoebaOptimizer.h"
#include "itkPowellOptimizer.h"
#include "itkFRPROptimizer.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkVersorRigid3DTransformOptimizer.h"
#include "itkConjugateGradientOptimizer.h"
#include "itkLBFGSOptimizer.h"
#include "itkSPSAOptimizer.h"

// general
#include <iomanip>

/*Requires at least this section in the associated *.ggo file: (modify defaults to fit application)

option "optimizer"	-	"0=Simplex, 1=Powell, 2=FRPR, 3=Regular Step GD, 4=VersorRigid3D, 5=Conjugated Gradient, 6=L-BFGS, 7=L-BFGS-B" int no default="0"
option "delta"		-	"0: Initial delta, otherwise automatic"    						double 	no
option "step" 		-       "1,2,3,4: Initial stepsize (to be multiplied with the gradient)" 			double	no	default="2.0"
option "relax" 		-       "3,4: Relaxation of the stepsize (multiplied each time the gradient changes sign)" 	double	no	default="0.7"
option "valueTol"	-	"0,1,2: Tolerance on the function"							double 	no	default="0.01"
option "stepTol"	-	"0,1,3,4: Tolerance on the step size"							double  no 	default="0.1"
option "gradTol"	-	"3,4,6,7: Tolerance on the (projected) gradient magnitude (7: 1=low->1e-10=high precision)"	double 	no	default="1e-5"
option "lineAcc"	- 	"6: Line accuracy (eg: high=0.1, low=0.9)" 	double	no 	default="0.9"
option "convFactor"	-	"7: Convergence factor: terminate if factor*machine_precision>reduction in cost (1e+12 low -> 1e+1 high precision) "	double		no	default="1e+7"
option "maxIt"		-	"0-7: Maximum number of iterations"					int 	no	default="500"
option "maxLineIt"	-	"1,2: Maximum number of line iterations"					int 	no	default="50"
option "maxEval"	-	"6,7: Maximum number of evaluations"						int	no	default="500"
option "maxCorr"	-	"7: Maximum number of corrections"						int	no	default="5"
option "selectBound"	-	"7: Select the type of bound: 0=none, 1=u, 2=u&l, 3=l"			int 	no	default="0"
option "lowerBound"	-	"7: The lower bound"							double 	no
option "upperBound"	-	"7: The upper bound"							double 	no

*/

namespace clitk
{
template<class args_info_type>
class GenericOptimizer : public itk::LightObject
{
public:
  //==============================================
  typedef GenericOptimizer     Self;
  typedef itk::LightObject     Superclass;
  typedef itk::SmartPointer<Self>            Pointer;
  typedef itk::SmartPointer<const Self>      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);


  typedef itk::SingleValuedNonLinearOptimizer OptimizerType;
  typedef OptimizerType::Pointer OptimizerPointer;

  //==============================================
  //Set members
  void SetArgsInfo(args_info_type args_info) {
    m_ArgsInfo= args_info;
    m_Verbose=m_ArgsInfo.verbose_flag;
  }
  void SetMaximize(bool f) {
    m_Maximize=f;
    m_MaximizeGiven=true;
  }

  void SetNumberOfParameters(unsigned int n) {
    m_NumberOfParameters=n;
  }

  //==============================================
  //Get members
  OptimizerPointer GetOptimizerPointer(void) {

    if (!m_MaximizeGiven)std::cerr<<"Warning: Maximize/Minimize was not given to the optimizer!!"<<std::endl;
    else if(m_Verbose) {
      if (m_Maximize)  std::cout<<"Maximizing similarity measure..."<<std::endl;
      else std::cout<<"Minimizing similarity measure..."<<std::endl;
    }


    //============================================================================
    //switch on  the  metric type chosen and set metric specific members
    switch (m_ArgsInfo.optimizer_arg) {

    case 0: {
      itk::AmoebaOptimizer::Pointer m =
        itk::AmoebaOptimizer::New();
      m_OptimizerIsVNL=true;

      //Set Parameters for this optimizer
      m->SetMaximize(m_Maximize);
      if (m_ArgsInfo.delta_given) {
        itk::Array<double> delta(m_NumberOfParameters);
        for (unsigned int i =0 ; i < m_NumberOfParameters; i ++)
          delta[i]=m_ArgsInfo.delta_arg;
        m->SetInitialSimplexDelta(delta);
        m->SetAutomaticInitialSimplex(false);
      } else m->SetAutomaticInitialSimplex(true);
      m->SetMaximumNumberOfIterations(m_ArgsInfo.maxIt_arg);
      m->SetFunctionConvergenceTolerance(m_ArgsInfo.valueTol_arg);
      m->SetParametersConvergenceTolerance(m_ArgsInfo.stepTol_arg);
      if (m_Verbose) std::cout<<"Using Simplex Optimizer..."<<std::endl;
      m_Optimizer=m;
      break;
    }

    case 1: {
      itk::PowellOptimizer::Pointer m =
        itk::PowellOptimizer::New();
      m_OptimizerIsVNL=false;

      //Set Parameters for this optimizer
      m->SetMaximize(m_Maximize);
      m->SetStepLength(m_ArgsInfo.step_arg);
      m->SetStepTolerance(m_ArgsInfo.stepTol_arg);
      m->SetValueTolerance (m_ArgsInfo.valueTol_arg);
      m->SetMaximumIteration(m_ArgsInfo.maxIt_arg);
      m->SetMaximumLineIteration(m_ArgsInfo.maxLineIt_arg);
      if (m_Verbose) std::cout<<"Using Powell Optimizer..."<<std::endl;
      m_Optimizer=m;
      break;
    }

    case 2: {
      itk::FRPROptimizer::Pointer m =
        itk::FRPROptimizer::New();
      m_OptimizerIsVNL=false;

      //Set Parameters for this optimizer
      m->SetMaximize(m_Maximize);
      m->SetStepLength(m_ArgsInfo.step_arg);
      m->SetStepTolerance(m_ArgsInfo.stepTol_arg);
      m->SetValueTolerance (m_ArgsInfo.valueTol_arg);
      m->SetMaximumIteration(m_ArgsInfo.maxIt_arg);
      m->SetMaximumLineIteration(m_ArgsInfo.maxLineIt_arg);
      if (m_Verbose) std::cout<<"Using Powell Optimizer..."<<std::endl;
      m_Optimizer=m;
      break;
    }

    case 3: {
      itk::RegularStepGradientDescentOptimizer::Pointer m =
        itk::RegularStepGradientDescentOptimizer::New();
      m_OptimizerIsVNL=false;

      //Set Parameters for this optimizer
      m->SetMaximize(m_Maximize);
      m->SetMinimumStepLength( m_ArgsInfo.stepTol_arg );
      m->SetRelaxationFactor(  m_ArgsInfo.relax_arg );
      m->SetMaximumStepLength( m_ArgsInfo.step_arg );
      m->SetNumberOfIterations(m_ArgsInfo.maxIt_arg  );
      m->SetGradientMagnitudeTolerance(m_ArgsInfo.gradTol_arg  );
      if (m_Verbose) std::cout<<"Using Regular Step Gradient Descent Optimizer..."<<std::endl;
      m_Optimizer=m;
      break;
    }

    case 4: {
      itk::VersorRigid3DTransformOptimizer::Pointer m
      = itk::VersorRigid3DTransformOptimizer::New();
      m_OptimizerIsVNL=false;

      //Set Parameters for this optimizer
      m->SetMaximize(m_Maximize);
      m->SetMinimumStepLength( m_ArgsInfo.stepTol_arg );
      m->SetRelaxationFactor(  m_ArgsInfo.relax_arg );
      m->SetMaximumStepLength( m_ArgsInfo.step_arg );
      m->SetNumberOfIterations(m_ArgsInfo.maxIt_arg  );
      m->SetGradientMagnitudeTolerance(m_ArgsInfo.gradTol_arg  );
      if (m_Verbose) std::cout<<"Using VersorRigid3DTransform Optimizer..."<<std::endl;
      m_Optimizer=m;
      break;
    }

    case 5: {

      itk::ConjugateGradientOptimizer::Pointer m
      = itk::ConjugateGradientOptimizer::New();
      m_OptimizerIsVNL=true;

      //Set Parameters for this optimizer
      m->SetMaximize(m_Maximize);
      if (m_Verbose) std::cout<<"Using Conjugated Gradient Optimizer..."<<std::endl;
      m_Optimizer=m;
      break;
    }

    case 6: {
      itk::LBFGSOptimizer::Pointer m
      = itk::LBFGSOptimizer::New();
      m_OptimizerIsVNL=true;

      //Set Parameters for this optimizer
      m->SetMaximize(m_Maximize);
      m->SetGradientConvergenceTolerance(m_ArgsInfo.gradTol_arg);
      m->SetLineSearchAccuracy(m_ArgsInfo.lineAcc_arg);
      m->SetMaximumNumberOfFunctionEvaluations(m_ArgsInfo.maxEval_arg);
      m->SetTrace(true);
      if (m_Verbose) std::cout<<"Using L-BFGS Optimizer..."<<std::endl;
      m_Optimizer=m;
      break;
    }

    case 7: {
      LBFGSBOptimizer::Pointer m
      = LBFGSBOptimizer::New();
      m_OptimizerIsVNL=true;

      //Set Parameters for this optimizer
      m->SetMaximize(m_Maximize);
      m->SetMaximumNumberOfEvaluations(m_ArgsInfo.maxEval_arg);
      m->SetMaximumNumberOfIterations(m_ArgsInfo.maxIt_arg);
      m->SetMaximumNumberOfCorrections(m_ArgsInfo.maxCorr_arg);
      m->SetCostFunctionConvergenceFactor( m_ArgsInfo.convFactor_arg);
      m->SetProjectedGradientTolerance( m_ArgsInfo.gradTol_arg);

      // Bounds
      clitk::LBFGSBOptimizer::BoundSelectionType boundSelect( m_NumberOfParameters );
      clitk::LBFGSBOptimizer::BoundValueType upperBound( m_NumberOfParameters );
      clitk::LBFGSBOptimizer::BoundValueType lowerBound( m_NumberOfParameters );
      boundSelect.Fill( m_ArgsInfo.selectBound_arg );
      upperBound.Fill( m_ArgsInfo.upperBound_arg );
      lowerBound.Fill( m_ArgsInfo.lowerBound_arg );
      m->SetBoundSelection( boundSelect );
      m->SetUpperBound( upperBound );
      m->SetLowerBound( lowerBound );

      if (m_Verbose) std::cout<<"Using L-BFGS-B Optimizer..."<<std::endl;
      m_Optimizer=m;
      break;
    }

    //     case 8:
    //       {
    // 	itk::SPSAOptimizer::Pointer m =
    // 	  itk::SPSAOptimizerr::Pointer::New();
    //      m_OptimizerIsVNL=false;

    // 	//Set Parameters for this optimizer
    // 	m->SetMaximize(m_Maximize);
    // 	m->SetGamma(m_ArgsInfo.gamma_arg);
    // 	m->SetAlpha(m_ArgsInfo.gamma_arg);
    // 	m->SetA(m_ArgsInfo.bigA_arg);
    // 	m->Seta(m_ArgsInfo.littleA_arg);
    // 	m->Setc(m_ArgsInfo.littleC_arg);
    // 	m->SetMaximumNumberOfIterations(m_ArgsInfo.maxIt_arg);
    // 	m->SetMinimumNumberOfIterations(m_ArgsInfo.minIt_arg);
    // 	m->SetNumberOfPerturbations(m_ArgsInfo.per_arg);
    // 	m->SetStateOfConvergenceDecayRate(args_inf.conv_arg);
    // 	if (m_Verbose) std::cout<<"Using SPSA Optimizer..."<<std::endl;
    // 	m_Optimizer=m;
    // 	break;
    //       }

    }


    //============================================================================
    //return the pointer
    return m_Optimizer;
  }


  void OutputIterationInfo(void) const {
    switch (m_ArgsInfo.optimizer_arg) {
    case 0: {
      itk::AmoebaOptimizer* o=dynamic_cast<itk::AmoebaOptimizer*>(m_Optimizer.GetPointer());
      std::cout<<std::setprecision(6);
      //if (m_OutputIteration)  std::cout<<"Iteration: "<< o->GetIteration()<<std::endl;
      if (m_OutputPosition)  std::cout<<"Position: "<< o->GetCachedCurrentPosition()<<std::endl;
      if (m_OutputValue)  std::cout<<"Value: "<< o->GetCachedValue()<<std::endl;
      // if (m_OutputGradient)  std::cout<<"Gradient: "<< o->GetCachedDerivative()<<std::endl;
      std::cout<<std::endl;
      break;
    }
    case 1: {
      itk::PowellOptimizer* o=dynamic_cast<itk::PowellOptimizer*>(m_Optimizer.GetPointer());
      std::cout<<std::setprecision(6);
      if (m_OutputIteration)  std::cout<<"Iteration: "<< o->GetCurrentIteration()<<std::endl;
      if (m_OutputIteration)  std::cout<<"Line Iteration: "<< o->GetCurrentLineIteration()<<std::endl;
      if (m_OutputPosition)  std::cout<<"Position: "<< o->GetCurrentPosition()<<std::endl;
      if (m_OutputValue)  std::cout<<"Value: "<< o->GetCurrentCost()<<std::endl;
      //if (m_OutputGradient)  std::cout<<"Gradient: "<< o->GetDerivative()<<std::endl;
      std::cout<<std::endl;
      break;
    }
    case 2: {
      itk::FRPROptimizer* o=dynamic_cast<itk::FRPROptimizer*>(m_Optimizer.GetPointer());
      std::cout<<std::setprecision(6);
      if (m_OutputIteration)  std::cout<<"Iteration: "<< o->GetCurrentIteration()<<std::endl;
      if (m_OutputIteration)  std::cout<<"Line Iteration: "<< o->GetCurrentLineIteration()<<std::endl;
      if (m_OutputPosition)  std::cout<<"Position: "<< o->GetCurrentPosition()<<std::endl;
      if (m_OutputValue)  std::cout<<"Value: "<< o->GetCurrentCost()<<std::endl;
      //if (m_OutputGradient)  std::cout<<"Gradient: "<< o->GetDerivative()<<std::endl;
      std::cout<<std::endl;
      break;
    }
    case 3: {
      itk::RegularStepGradientDescentOptimizer* o=dynamic_cast<itk::RegularStepGradientDescentOptimizer*>(m_Optimizer.GetPointer());
      std::cout<<std::setprecision(6);
      if (m_OutputIteration)  std::cout<<"Iteration: "<< o->GetCurrentIteration()<<std::endl;
      if (m_OutputPosition)  std::cout<<"Position: "<< o->GetCurrentPosition()<<std::endl;
      if (m_OutputPosition)  std::cout<<"Step length: "<< o->GetCurrentStepLength()<<std::endl;
      if (m_OutputValue)  std::cout<<"Value: "<< o->GetValue()<<std::endl;
      if (m_OutputGradient)  std::cout<<"Gradient: "<< o->GetGradient()<<std::endl;
      std::cout<<std::endl;
      break;
    }
    case 4: {
      itk::VersorRigid3DTransformOptimizer* o=dynamic_cast<itk::VersorRigid3DTransformOptimizer*>(m_Optimizer.GetPointer());
      std::cout<<std::setprecision(6);
      if (m_OutputIteration)  std::cout<<"Iteration: "<< o->GetCurrentIteration()<<std::endl;
      if (m_OutputPosition)  std::cout<<"Position: "<< o->GetCurrentPosition()<<std::endl;
      if (m_OutputPosition)  std::cout<<"Step length: "<< o->GetCurrentStepLength()<<std::endl;
      if (m_OutputValue)  std::cout<<"Value: "<< o->GetValue()<<std::endl;
      if (m_OutputGradient)  std::cout<<"Gradient: "<< o->GetGradient()<<std::endl;
      std::cout<<std::endl;
      break;
    }
    case 5: {
      itk::ConjugateGradientOptimizer* o=dynamic_cast<itk::ConjugateGradientOptimizer*>(m_Optimizer.GetPointer());
      std::cout<<std::setprecision(6);
      if (m_OutputIteration)  std::cout<<"Iteration: "<< o->GetCurrentIteration()<<std::endl;
      if (m_OutputPosition)  std::cout<<"Position: "<< o->GetCachedCurrentPosition()<<std::endl;
      if (m_OutputValue)  std::cout<<"Value: "<< o->GetCachedValue()<<std::endl;
      if (m_OutputGradient)  std::cout<<"Gradient: "<< o->GetCachedDerivative()<<std::endl;
      std::cout<<std::endl;
      break;
    }
    case 6: {
      itk::LBFGSOptimizer* o=dynamic_cast<itk::LBFGSOptimizer*>(m_Optimizer.GetPointer());
      std::cout<<std::setprecision(6);
      //if (m_OutputIteration)  std::cout<<"Iteration: "<< o->GetCurrentIteration()<<std::endl;
      if (m_OutputPosition)  std::cout<<"Position: "<< o->GetCachedCurrentPosition()<<std::endl;
      if (m_OutputValue)  std::cout<<"Value: "<< o->GetCachedValue()<<std::endl;
      //	  if (m_OutputValue)  std::cout<<"Norm Projected Gradient: "<< o->GetInfinityNormOfProjectedGradient()<<std::endl;
      if (m_OutputGradient)  std::cout<<"Gradient: "<< o->GetCachedDerivative()<<std::endl;
      std::cout<<std::endl;
      break;
    }
    case 7: {
      clitk::LBFGSBOptimizer* o=dynamic_cast<clitk::LBFGSBOptimizer*>(m_Optimizer.GetPointer());
      std::cout<<std::setprecision(6);
      if (m_OutputIteration)  std::cout<<"Iteration: "<< o->GetCurrentIteration()<<std::endl;
      if (m_OutputPosition)  std::cout<<"Position: "<< o->GetCachedCurrentPosition()<<std::endl;
      if (m_OutputValue)  std::cout<<"Value: "<< o->GetValue()<<std::endl;
      if (m_OutputValue)  std::cout<<"Norm Projected Gradient:"<< o->GetInfinityNormOfProjectedGradient()<<std::endl;
      if (m_OutputGradient)  std::cout<<"Gradient: "<< o->GetCachedDerivative()<<std::endl;
      std::cout<<std::endl;
      break;
    }
    //       case 8:
    // 	{
    // 	  itk::VersorRigid3DTransformOptimizer* o=dynamic_cast<itk::VersorRigid3DTransformOptimizer*>(m_Optimizer.GetPointer());
    // 	  std::cout<<std::setprecision(6);
    // 	  if (m_OutputIteration)  std::cout<<"Iteration: "<< o->GetIteration()<<std::endl;
    // 	  if (m_OutputPosition)  std::cout<<"Position: "<< o->GetCurrentPosition()<<std::endl;
    // 	  if (m_OutputValue)  std::cout<<"Value: "<< o->GetCurrentCost()<<std::endl;
    // 	  if (m_OutputGradient)  std::cout<<"Gradient: "<< o->GetDerivative()<<std::endl;
    // 	  std::cout<<std::endl;
    // 	}
    }
  }

  void SetOutputIteration(bool o) {
    m_OutputIteration=o;
  }
  void SetOutputPosition(bool o) {
    m_OutputPosition=o;
  }
  void SetOutputValue(bool o) {
    m_OutputValue=o;
  }
  void SetOutputGradient(bool o) {
    m_OutputGradient=o;
  }


  //==============================================
protected:
  GenericOptimizer() {
    m_Optimizer=NULL;
    m_Maximize=false;
    m_Verbose=false;
    m_MaximizeGiven=false;
    m_NumberOfParameters=1;
    m_OptimizerIsVNL=false;
    m_OutputIteration=true;
    m_OutputPosition=false;
    m_OutputValue=true;
    m_OutputGradient=false;

  }
  ~GenericOptimizer() {};

private:
  args_info_type m_ArgsInfo;
  OptimizerPointer m_Optimizer;
  bool m_Maximize;
  bool m_Verbose;
  bool m_MaximizeGiven;
  unsigned int m_NumberOfParameters;
  bool m_OptimizerIsVNL;
  bool m_OutputIteration;
  bool m_OutputPosition;
  bool m_OutputValue;
  bool m_OutputGradient;
};

} // end namespace clitk

#endif // #define __clitkGenericOptimizer_h
