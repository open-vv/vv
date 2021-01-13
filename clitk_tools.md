This page contains additional information on how to use clitk tools:




**clitk4DImageToNVectorImage**

clitk4DImageToNVectorImage 1.0

Convert the 4th dimension of a 4D Image into a VectorPixel 3D Image

Usage: clitk4DImageToNVectorImage [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help          | Print help and exit    |
| -V | --version       | Print version and exit |
|   | --config=STRING | Config file            |
| -v | --verbose       | Verbose  (default=off) |
| -i | --input=STRING  | Input image filename   |
| -o | --output=STRING | Output image filename  |


**clitkAffineRegistration**

clitk 1.0

Perform an affine registration between two images.

Usage: clitk [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Run Time: | 
| -v | --verbose | Verbose  (default=off) | 
|   | --gradient | If verbose, show gradient at each iteration  (default=off) | 
|   | --threads=INT | Number of threads to use  (default=min(#cores,8)) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Input (Both images have to be of the same dimension (2 or 3D). For 2D-3D  | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">(non-projective) registrations, give the 2D image a third dimension of 1 and  | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">set it to the reference image.): | 
| -i | --reference=STRING | Reference or fixed image filename | 
| -j | --target=STRING | Target or moving image filename | 
| -m | --referenceMask=STRING | Mask to placed over the reference image | 
|   | --targetMask=STRING | Mask to placed over the target image | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Output: | 
| -o | --output=STRING | Transformed object image filename | 
|   | --checker_after=STRING | Checherboard representation of the transformed object image and reference image | 
|   | --checker_before=STRING | Checherboard representation of the object image and reference image | 
|   | --after=STRING | Difference between the reference image and the transformed object | 
|   | --before=STRING | Difference between the reference image and the original object image | 
|   | --matrix=STRING | Affine matrix (reference to object space) filename  | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Interpolator: | 
|   | --interp=INT | Interpolation: 0=NN, 1=Linear, 2=BSpline, 3=BLUT  (default=`1') | 
|   | --interpOrder=INT | Order if BLUT or BSpline (0-5)  (default=`3') | 
|   | --interpSF=INT | Sampling factor if BLUT  (default=`20') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Transform (Input and Output transformation parameters map the physical space  | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">of the fixed or reference image into the physical space of the moving or object  | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">image. Positive rotations result in a counter-clockwise rotation for the moving  | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">image. Positive translations result in shift along the negative axis for the  | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">moving image.): | 
|   | --transform=INT | Type: 0=Identity, 1=Translation, 2=Rigid, 3=Affine  (default=`2') | 
| -x | --transX=FLOAT | 1-3: Initial translation in mm along the X axis (default=`0.0') | 
| -y | --transY=FLOAT | 1-3: Initial translation in mm along the Y axis (default=`0.0') | 
| -z | --transZ=FLOAT | 1-3: Initial translation in mm along the Z axis (default=`0.0') | 
|   | --initMatrix=STRING | 1-3: Initial matrix (reference to object space) filename  | 
|   | --moment | 1-3: Initialize translation by aligning the center of gravities for the respective intensities  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Metric (optimized, threaded versions are available for *, compile ITK with  | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">REVIEW and OPT_REGISTRATION enabled): | 
|   | --metric=INT | Type: 0=Mean-Squares*, 1=Normalized CC*, 2=Histogram CC, 3=Gradient-Difference, 4=Viola-Wells MI, 5=Histogram MI, 6=Mattes' MI*, 7=Normalized MI, 8=CR, 9=SSD for BLUT FFD**  (default=`0') | 
|   | --samples=FLOAT | Specify fraction [0, 1] of samples of the reference image used for the metric (* only). Use high fraction for detailed images (eg. 0.2, 0.5), for smooth images 0.01 might be enough.  (default=`1') | 
|   | --intThreshold=FLOAT | Fixed image samples intensity threshold (* only; caution with --normalize) | 
|   | --subtractMean | 1: Subtract mean for NCC calculation (narrows optimal)  (default=on) | 
|   | --bins=INT | 2,5-8: Number of histogram bins  (default=`50') | 
|   | --random | 4,6: Samples should be taken randomly, otherwise uniformly  (default=off) | 
|   | --stdDev=FLOAT | 4: specify the standard deviation in mm of the gaussian kernels for both PDF estimations  (default=`0.4') | 
|   | --explicitPDFDerivatives | 6: Calculate PDF derivatives explicitly (affine=true; FFD=false)  (default=on) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Preprocessing: | 
|   | --normalize | Normalize images before registration (not necessary for metric 1,2,5,6,8; caution with  | 
|   | --intThreshold) | (default=off) | 
|   | --blur=FLOAT | Blur images before registration, use Gaussian with std dev (none by default)   (default=`0.0') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Optimizer: | 
|   | --optimizer=INT | 0=Simplex, 1=Powell, 2=FRPR, 3=Regular Step GD, 4=VersorRigid3D, 5=Conjugated Gradient, 6=L-BFGS, 7=L-BFGS-B  (default=`0') | 
|   | --delta=DOUBLE | 0: Initial delta, otherwise automatic | 
|   | --step=DOUBLE | 1,2,3,4: Initial stepsize (to be multiplied with the gradient)  (default=`2.0') | 
|   | --relax=DOUBLE | 3,4: Relaxation of the stepsize (multiplied each time the gradient changes sign)  (default=`0.7') | 
|   | --valueTol=DOUBLE | 0,1,2: Tolerance on the function  (default=`0.01') | 
|   | --stepTol=DOUBLE | 0,1,3,4: Tolerance on the step size  (default=`0.1') | 
|   | --gradTol=DOUBLE | 3,4,6,7: Tolerance on the (projected) gradient magnitude (7: 1=low->1e-10=high precision)  (default=`1e-5') | 
|   | --lineAcc=DOUBLE | 6: Line accuracy (eg: high=0.1, low=0.9)  (default=`0.9') | 
|   | --convFactor=DOUBLE | 7: Convergence factor: terminate if factor*machine_precision>reduction in cost (1e+12 low, 1e+7 moderate and 1e+1 high precision)   (default=`1e+12') | 
|   | --maxIt=INT | 0-7: Maximum number of iterations  (default=`500') | 
|   | --maxLineIt=INT | Maximum number of line iterations  (default=`50') | 
|   | --maxEval=INT | Maximum number of evaluations  (default=`500') | 
|   | --maxCorr=INT | Maximum number of corrections  (default=`5') | 
|   | --selectBound=INT | 7: Select the type of bound: 0=none, 1=u, 2=u&l, 3=l  (default=`0') | 
|   | --lowerBound=DOUBLE | 7: The lower bound | 
|   | --upperBound=DOUBLE | 7: The upper bound | 
|   | --rWeight=FLOAT | Weight of 1° of rotation during optimisation (high weight, less change)  (default=`50.0') | 
|   | --tWeight=FLOAT | Weight of 1mm of translation  during optimisation (high weight, less change)  (default=`1.0') | 
|   | --levels=INT | Number of resolution levels  (default=`1') | 
|   | --inc=FLOAT | Increment factor (x)  previous step/tol = new step/tol at next resolution level  (default=`1.2') | 
|   | --dec=FLOAT | Decrement factor (:)  previous step/tol = new step/tol at next resolution level  (default=`4') | 


**clitkAffineTransform**

clitkAffineTransform 1.0

Resample with or without affine transform of 2D, 3D, 4D images or vector fields

Usage: clitkAffineTransform [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">I/O: | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -l | --like=STRING | Resample output this image (size, spacing, origin, direction) | 
|   | --transform_grid | Apply affine transform to input grid for output's  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Options: | 
|   | --size=INT | New output size if different from input | 
|   | --spacing=DOUBLE | New output spacing if different from input | 
|   | --spacinglike=STRING | New output spacing like this image | 
|   | --origin=DOUBLE | New output origin if different from input | 
|   | --direction=DOUBLE | New output direction if different from input | 
| -m | --matrix=STRING | Affine matrix (homogene) filename | 
| -e | --elastix=STRING | Read EulerTransform from elastix output file (combine if multiple) | 
| -r | --rotate=DOUBLE | Rotation to apply (radians) | 
| -t | --translate=DOUBLE | Translation to apply (mm) | 
|   | --pad=DOUBLE | Edge padding value  (default=`0.0') | 
|   | --adaptive | Adapt the size, spacing or the origin when one of the previous tag is on (use previous clitkResampleImage)  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Interpolation: | 
|   | --interp=INT | Interpolation: 0=NN, 1=Linear, 2=BSpline, 3=BLUT  (default=`1') | 
|   | --interpOrder=INT | Order if BLUT or BSpline (0-5)  (default=`3') | 
|   | --interpSF=INT | Sampling factor if BLUT  (default=`20') | 
|   | --interpVF=INT | Interpolation: 0=NN, 1=Linear, 2=BSpline, 3=BLUT  (default=`1') | 
|   | --interpVFOrder=INT | Order if BLUT or BSpline (0-5)  (default=`3') | 
|   | --interpVFSF=INT | Sampling factor if BLUT  (default=`20') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Gaussian filtering: | 
| -g | --gauss=DOUBLE | Apply gaussian filter before (sigma in mm) (default=0.0) | 
|   | --autogauss | Apply gaussian filter before with auto sigma when downsampling (default=off)  (default=off) | 


**clitkAnisotropicDiffusion**

clitkAnisotropicDiffusion 1.0

Performs gradient or curvature anisotropic diffusion filtering

Usage: clitkAnisotropicDiffusion [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
|   | --type=ENUM | Type of filtering  (possible values="Gradient", "Curvature" default=`Gradient') | 
| -n | --niterations=INT | Number of iterations  (default=`5') | 
| -c | --conductance=DOUBLE | Conductance  (default=`3') | 
| -t | --timestep=DOUBLE | Time step  (default=`0.01') | 


**clitkAutoCrop**

clitkAutoCrop 1.0

Crop a mask image

Usage: clitkAutoCrop [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -v | --verbose | Verbose  (default=off) | 
|   | --verboseOption | Display options values  (default=off) | 
|   | --verboseWarningOff | Do not display warning  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">I/O: | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
|   | --BG=DOUBLE | Background value in input.  (default=`0') | 


**clitkBackProjectImage**

clitk Back project a 2D image with cone-beam geometry

Usage: clitk [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Run Time: | 
| -v | --verbose | Verbose  (default=off) | 
|   | --threads=INT | Number of threads to use  (default=min(cores,8)) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Input: | 
| -i | --input=STRING | Input grid filename | 
| -o | --output=STRING | Output grid filename | 
| -m | --mask=STRING | 3D Mask in which the filter should be applied (not supported yet;-) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Projection Parameters: | 
|   | --iso=FLOAT | The isocenter  (default=`0.0') | 
|   | --screen=FLOAT | Specify the source to screen distance in mm  (default=`1536.0') | 
|   | --axis=FLOAT | Specify the source to axis distance in mm  (default=`1000.0') | 
|   | --angle=FLOAT | Specify the projection angle  (default=`0.0') | 
|   | --matrix=STRING | Rigid tranform prior to projection (4x4) | 
|   | --pad=FLOAT | Padding value  (default=`0.0') | 
|   | --panel_shift=DOUBLE | Precise position of the panel in mm | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Output Image: | 
|   | --like=STRING | Make output like this image | 
|   | --origin=DOUBLE | Origin for the output image  (default=`0.0') | 
|   | --size=INT | Size for the output image  (default=`100') | 
|   | --spacing=DOUBLE | Spacing for the output image  (default=`1.0') | 


**clitkBinarizeImage**

clitkBinarizeImage 1.0

Usage: clitkBinarizeImage [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -l | --lower=DOUBLE | Lower intensity (default=min), fg is greater than this value | 
| -u | --upper=DOUBLE | Upper intensity (default=max), fg is lower than this value | 
|   | --fg=DOUBLE | Foreground (FG) or 'inside' value  (default=`1') | 
|   | --bg=DOUBLE | Background (BG) or 'ouside' value  (default=`0') | 
|   | --mode=STRING | Use FG and/or BG values (if FG, the BG is replaced by the input image values)  (possible values="FG", "BG", "both" default=`both') | 
| -p | --percentage=DOUBLE | Percentage of total pixels values (in %) | 


**clitkBlurImage**

clitkBlurImage 1.0

Filter the input image with a Gaussian

Usage: clitkBlurImage [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
|   | --variance=DOUBLE | value of the gaussian variance (multiple values=number of image dimension) in mm² - default=1.0 | 


**clitkBLUTDIR**

clitk Register 2 images using optimized BLUT FFD...

Usage: clitk [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Run Time: | 
| -v | --verbose | Verbose  (default=off) | 
|   | --threads=INT | Number of threads to use (default=min(#cores,8)) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Input: | 
| -r | --reference=STRING | Input reference 3D image (float) | 
| -t | --target=STRING | Input target	 2D image (float) | 
| -m | --referenceMask=STRING | Mask or labels to placed over the reference image | 
|   | --targetMask=STRING | Mask to placed over the target image | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Output: | 
|   | --vf=STRING | Result DVF | 
|   | --coeff=STRING | Result coefficient images | 
|   | --coeffEveryN=INT | Result coefficient images at every N iterations (requires --coeff and --verbose)  (default=`20') | 
| -o | --output=STRING | Deformed target image | 
|   | --before=STRING | Difference image before (but after rigid transform) | 
|   | --after=STRING | Difference image after  | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Transform (Note that only one of --control, --spacing is required. The other  | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">will be adjusted to fit the region and allow exact representation.  | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">SamplingFactor will be set accordingly: | 
|   | --initMatrix=STRING | Prior rigid/affine transform matrix from reference to target space | 
|   | --centre | Centre images before starting registration (ignored if initMatrix was given)  (default=off) | 
|   | --initCoeff=STRING | Initial coefficient image | 
|   | --order=INT | Spline Order FFD  (default=`3') | 
|   | --control=INT | Internal control points for each dimension | 
|   | --spacing=DOUBLE | Control point spacing for each dimension (mm) | 
|   | --samplingFactor=INT | LUT sampling factor | 
|   | --itkbspline | Use ITK BSpline instead of multilabel BLUT for debug  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Interpolator: | 
|   | --interp=INT | Interpolation: 0=NN, 1=Linear, 2=BSpline, 3=BLUT  (default=`1') | 
|   | --interpOrder=INT | Order if BLUT or BSpline (0-5)  (default=`3') | 
|   | --interpSF=INT | Sampling factor if BLUT  (default=`20') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Metric (optimized, threaded versions are available for *, compile ITK with  | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">REVIEW and OPT_REGISTRATION enabled. Further optimized versions ** for  BLUT  | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">FFD optimizing a !3D! vector field): | 
|   | --metric=INT | Type: 0=SSD*, 1=Normalized CC*, 2=Histogram CC, 3=Gradient-Difference, 4=Viola-Wells MI, 5=Histogram MI, 6=Mattes' MI*, 7=Normalized MI, 8=CR, 9=SSD for BLUT FFD**, 10=CC for BLUT FFD**, 11=Mattes' MI for BLUT FFD**  (default=`0') | 
|   | --samples=FLOAT | Specify fraction [0, 1] of samples of the reference image used for the metric (* only). Use high fraction for detailed images (eg. 0.2, 0.5), for smooth images 0.01 might be enough.  (default=`1') | 
|   | --intThreshold=FLOAT | Fixed image samples intensity threshold (* only) | 
|   | --subtractMean | 1: Subtract mean for NCC calculation (narrows optimal)  (default=on) | 
|   | --bins=INT | 2,5-8: Number of histogram bins  (default=`50') | 
|   | --random | 4,6: Samples should be taken randomly, otherwise uniformly  (default=off) | 
|   | --stdDev=FLOAT | 4: specify the standard deviation in mm of the gaussian kernels for both PDF estimations  (default=`0.4') | 
|   | --explicitPDFDerivatives | 6: Calculate PDF derivatives explicitly (rigid=true; FFD=false)  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Optimizer: | 
|   | --optimizer=INT | 0=Simplex, 1=Powell, 2=FRPR, 3=Regular Step GD, 4=VersorRigid3D, 5=Conjugated Gradient, 6=L-BFGS, 7=L-BFGS-B  (default=`7') | 
|   | --delta=DOUBLE | 0: Initial delta, otherwise automatic | 
|   | --step=DOUBLE | 1,2,3,4: Initial stepsize (to be multiplied with the gradient)  (default=`2.0') | 
|   | --relax=DOUBLE | 3,4: Relaxation of the stepsize (multiplied each time the gradient changes sign)  (default=`0.7') | 
|   | --valueTol=DOUBLE | 0,1,2: Tolerance on the function  (default=`0.01') | 
|   | --stepTol=DOUBLE | 0,1,3,4: Tolerance on the step size  (default=`0.1') | 
|   | --gradTol=DOUBLE | 3,4,6,7: Tolerance on the (projected) gradient magnitude (7: 1=low->1e-10=high precision)  (default=`1e-5') | 
|   | --lineAcc=DOUBLE | 6: Line accuracy (eg: high=0.1, low=0.9)  (default=`0.9') | 
|   | --convFactor=DOUBLE | 7: Convergence factor: terminate if factor*machine_precision>reduction in cost (1e+12 low -> 1e+1 high precision)   (default=`1e+7') | 
|   | --maxIt=INT | 0-7: Maximum number of iterations  (default=`500') | 
|   | --maxLineIt=INT | 1,2: Maximum number of line iterations  (default=`50') | 
|   | --maxEval=INT | 6,7: Maximum number of evaluations  (default=`500') | 
|   | --maxCorr=INT | 7: Maximum number of corrections  (default=`5') | 
|   | --selectBound=INT | 7: Select the type of bound: 0=none, 1=u, 2=u&l, 3=l  (default=`0') | 
|   | --lowerBound=DOUBLE | 7: The lower bound  (default=`0.0') | 
|   | --upperBound=DOUBLE | 7: The upper bound  (default=`0.0') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Registration: | 
|   | --levels=INT | Number of resolution levels  (default=`1') | 
|   | --skipLastPyramidLevel | Skip full resolution pyramid level  (default=off) | 
|   | --intermediate=STRING | Write the coefficient image of the intermediate levels (provide levels filenames) | 


**clitkCalculateTRE**

clitkCalculateTRE 1.0

Calculate TRE: give a reference point list and 1 or more target point lists. In 
case of the latter, the number of DVF (or the size of the 4th D) given should 
match the number of lists. Use --shipFirst to skip the first phase of a 4D DVF. 
General summarizes for all points (mean, SD, max) in magnitude and par 
component. Tre and warp gives all values for all points. 

Usage: clitkCalculateTRE [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Input: | 
|   | --ref=STRING | List of points in reference | 
| -i | --input=STRING | Lists of points in targets | 
|   | --vf=STRING | Input deformation fields | 
|   | --coeff=STRING | Input coefficient images | 
|   | --skip=INT | Skip a phase of a 4D DVF | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Interpolation: | 
|   | --interpVF=INT | Interpolation: 0=NN, 1=Linear, 2=BSpline, 3=BLUT  (default=`1') | 
|   | --interpVFOrder=INT | Order if BLUT or BSpline (0-5)  (default=`3') | 
|   | --interpVFSF=INT | Sampling factor if BLUT  (default=`20') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Output: | 
|   | --general=STRING | Summarized TRE values (mean, SD, max) | 
|   | --original=STRING | Original distances: base filename | 
|   | --originalMag=STRING | Original magnitude of distances: base filename | 
|   | --displacement=STRING | Estmated displacements: base filename | 
|   | --displacementMag=STRING | Estimated magnitude displacements: base filename | 
|   | --tre=STRING | All TRE values: base filename | 
|   | --treMag=STRING | All TRE magnitude values: base filename | 
|   | --warp=STRING | All warped points: base filename | 


**clitkCatImage**

clitkCatImage 1.0

Concatenate two images along chosen direction

Usage: clitkCatImage [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -i | --input1=STRING | First input image filename | 
| -j | --input2=STRING | Second input image filename | 
| -o | --output=STRING | Output image filename | 
| -d | --dimension=INT | Dimension on which to concatenate | 


**clitkChangeDicomTag**

clitkChangeDicomTag 1.0

Usage: clitkChangeDicomTag [OPTIONS]... [FILES]...

Change Dicom tag -k value to -t

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input dicom file | 
| -o | --output=STRING | Output dicom file | 
| -k | --key=STRING | Keys of tags to modify  (default=`0008|103e') | 
| -t | --tag=STRING | Tags values  (default=`MIDPOSITION') | 


**clitkChangeImageInfo**

clitkChangeImageInfo 1.0

Change the information (spacing, origin, direction...) of an image

Usage: clitkChangeImageInfo [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -s | --spacing=DOUBLE | Spacing | 
|   | --origin=DOUBLE | Origin | 
| -d | --direction=DOUBLE | Direction | 


**clitkComposeVF**

clitk Read two vector fields (.mhd, .vf, ..) and compose them using linear interpolation

Usage: clitk [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -i | --input1=STRING | Input1 VF filename | 
| -j | --input2=STRING | Input2 VF filename | 
| -o | --output=STRING | Output VF filename | 
| -t | --type=INT | Type of input images (0: DVF, 1: b-spline coefficient images)  (default=`0') | 
| -l | --like=STRING | Image to read output parameters from (obligatory if type=1) | 
| -p | --pad=FLOAT | Edgepadding value  (default=`0.0') | 
| -v | --verbose | Verbose  (default=off) | 


**clitkConnectedComponentLabeling**

clitkConnectedComponentLabeling 1.0

Basic segmentation : connected component labeling 

Usage: clitkConnectedComponentLabeling [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
|   | --verboseOption | Display options values  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">I/O: | 
| -i | --input=STRING | Input image filename | 
|   | --inputBG=INT | Input Background  (default=`0') | 
| -o | --output=STRING | Output filename | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Labelize: | 
|   | --minSize=INT | Minimum component size in voxels  (default=`100') | 
|   | --full | Full connecticity  (default=off) | 


**clitkConvertBSplineDeformableTransformToVF**

clitkConvertBSplineDeformableTransformToVF 1.0

Convert a BSpline transform to a DVF with given properties

Usage: clitkConvertBSplineDeformableTransformToVF [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">IO: | 
| -i | --input=STRING | Input BLUT-coefficient image filename | 
| -o | --output=STRING | Output image filename | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Output Image Properties: | 
|   | --like=STRING | Make output like this image | 
|   | --origin=DOUBLE | Origin for the output image  (default=`0.0') | 
|   | --size=INT | Size for the output image  (default=`100') | 
|   | --spacing=DOUBLE | Spacing for the output image  (default=`1.0') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Transform: | 
| -t | --type=INT | Type (0: itk bspline; 1: BLUT)  (default=`0') | 
|   | --order=INT | 1: Spline order | 
|   | --mask=STRING | 1: Mask image filename | 
|   | --shape=INT | 1: Transform shape: 0=egg, 1=diamond  (default=`0') | 


**clitkCropImage**

clitkCropImage 1.0

Crop an image according to a given extends or AutoCrop with a background value 
or like another image.
In all cases, by default, the output origin is set to be able to overlay input 
and output without registration.

Usage: clitkCropImage [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -v | --verbose | Verbose  (default=off) | 
|   | --verboseOption | Display options values  (default=off) | 
|   | --verboseWarningOff | Do not display warning  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">I/O: | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Used determined crop: | 
| -b | --boundingBox=INT | Bounding box of the crop region in pixel (in 3D: =x1,x2,y1,y2,z1,z2) | 
| -l | --lower=INT | Size of the lower crop region (multiple values) | 
| -u | --upper=INT | Size of the upper crop region (multiple values) | 
|   | --origin | Set new origin to zero  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">AutoCrop with BG value: | 
|   | --BG=DOUBLE | Background value in input.  (default=`0') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Crop like another image: | 
|   | --like=STRING | Crop like this image (must have the same spacing | 
|   | --updateOrigin | By default, with like image, the output origin is the same than the like image. Set to on the flag to keep the origin.  (default=off) | 
|   | --BGLike=DOUBLE | Background value in output (if like larger)  (default=`0') | 


**clitkDecomposeAndReconstruct**

clitkDecomposeAndReconstruct 1.0

Decompose through erosion, and reconstruct through dilation. Erode a binary 
image to decompose into different labels, keeping a record of the eroded 
regions (=erosion padding value). Dilate all labels of a label image  
separately, only dilating the marked regions (erosion padding value).

Usage: clitkDecomposeAndReconstruct [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">I/O: | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Parameters: | 
| -t | --type=INT | 0=Erode to decompose(binary image) , 1=Dilate to reconstuct(label image) , 2= 1(0(x))  (default=`0') | 
| -f | --full | Use full connectivity (1 side connected enough)  (default=on) | 
|   | --fg=FLOAT | 0: Foreground value (erode only this label) | 
|   | --bg=FLOAT | 1: Background value (ignore this label)  (default=`0') | 
|   | --pad=FLOAT | 0,1: The erosion padding value  (default=`-1') | 
| -b | --bound | 0-1: Set borders to foreground  (default=off) | 
| -r | --radius=INT | Use binary ball element with given radius  (default=`1') | 
| -n | --new=INT | 0,2: Erode till at least n new labels appear  (default=`1') | 
|   | --max=INT | 1,2: Consider only the n largest labels (rest will be put to fg)  (default=`10') | 
|   | --min=INT | 0,2: Minimum number of erosions  (default=`1') | 
|   | --minSize=INT | 0,2: Minimum object size  (default=`10') | 


**clitkDemonsDeformableRegistration**

clitk Apply Demons registration between (2D or 3D) images...

Usage: clitk [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Run Time: | 
| -v | --verbose | Verbose  (default=off) | 
|   | --threads=INT | Number of threads to use for intensive algorithms (default=min(cores,8)) | 
|   | --debug | Give debug info  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Input: | 
| -r | --reference=STRING | Input reference image | 
| -t | --target=STRING | Input target image | 
|   | --init=STRING | Input initial deformation field | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Output: | 
|   | --vf=STRING | Result DVF | 
| -o | --output=STRING | Deformed target image | 
|   | --before=STRING | Difference image before  | 
|   | --after=STRING | Difference image after  | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Demons: | 
|   | --demons=INT | Type: 0=normal, 1=symm, 2=fast symm, 3=diffeomorphic  (default=`3') | 
|   | --levels=INT | Number of resolution levels  (default=`1') | 
|   | --maxIter=INT | Maximum number of iterations at each resolution level  (default=`50') | 
|   | --maxRMSError=DOUBLE | Maximum RMS error at each resolution level  (default=`1') | 
|   | --stop=INT | Maximum oscillations at each resolution level (-1=unset)  (default=`-1') | 
|   | --sd=DOUBLE | SD (in mm) for smoothing in each dimension  (default=`2.0') | 
|   | --maxStep=DOUBLE | 2,3: Maximum step size (mm)  (default=`2') | 
|   | --scaleSD | Scale SD to coarse resolution levels  (default=off) | 
|   | --scaleStep | 2,3: Scale maximum step size to coarse resolution levels  (default=off) | 
|   | --fluid | Smooth update field instead of deformation field  (default=off) | 
|   | --spacing | Use image spacing for derivatives (non-isotropic voxels)  (default=off) | 
|   | --intThreshold=DOUBLE | 0,1,2: Intensity threshold to consider intensity equal  (default=`0.001') | 
|   | --movGrad | 1: Use moving image gradient  (default=off) | 
|   | --gradType=INT | 2,3: 0=Symmetric, 1=fixed, 2=warpedMoving, 3=mappedMoving  (default=`0') | 
|   | --firstOrder | 3: Use first order approx for exponential  (default=off) | 


**clitkDice**

clitkDice 1.0

Compute Dice between labeled images. Background must be 0.

Usage: clitkDice [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">General options: | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -l | --long | Long display (not only dice)  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Input: | 
| -i | --input1=STRING | Input mask 1 | 
| -j | --input2=STRING | Input mask 2 | 
|   | --label1=INT | Label in input1  (default=`1') | 
|   | --label2=INT | Label in input2  (default=`1') | 


**clitkDicom2Image**

clitk Try to convert a DICOM into an image (.hdr, .vox...) identifying all available series

Usage: clitk [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -t | --tolerance=DOUBLE | Tolerance for slice position  (default=`0') | 
| -o | --output=STRING | Output image filename | 
|   | --std_input | Take the like of input file from stdin, to support huge lists of filenames  (default=off) | 
|   | --focal_origin | Output files with FOCAL-like origin, instead of the origin present in the dicom files  (default=off) | 
| -p | --patientSystem | Open the image with patient coordinate system  (default=off) | 
| -n | --instanceNumber | Sort the images regarding instance number in dicom tag  (default=off) | 
| -r | --reverse | Reverse the slice order  (default=off) | 


**clitkDicomInfo**

clitk read and print Dicom header

Usage: clitk [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -s | --studyID | Only display Study UID  (default=off) | 
| -u | --uniq | Do not display if same study UID  (default=off) | 
| -f | --filename | Display filename  (default=off) | 


**clitkDicomRTPlan2Gate**

clitk 1.0

Convert DicomRTPlan to Gate compatible file

Usage: clitk [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | config file | 
| -i | --input=STRING | Input dicom file name | 
| -o | --output=STRING | Output text file name | 
| -v | --verbose | verbose  (default=off) | 


**clitkDicomRTStruct2Image**

clitk 1.0

Usage: clitk [OPTIONS]... [FILES]...

Convert DICOM RT Structure Set (contours) to binary image

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --verboseFile | Verbose file content  (default=off) | 
| -i | --input=STRING | Input Dicom file | 
| -j | --image=STRING | Used to read image info (spacing, origin) | 
| -o | --output=STRING | Output image base filename (roi number and extension will be append)Group: ROIoptionan option of this group is required | 
| -r | --roi=INT | ROI to binarize (if -1 = all roi)  (default=`-1') | 
| -n | --roiName=STRING | ROI name to binarize (be wary of spaces in ROI names; if blank, use given 'roi' value)  (default=`') | 
| -s | --roiNameSubstr=STRING | Substring of ROI name to binarize (reuturns all matches; if blank, use given 'roiName' value)  (default=`') | 
| -c | --crop | Crop binary mask  (default=off) | 
|   | --mha | Write the RTStruct as a mha image to avoid special character problems  (default=off) | 
|   | --vtk | Write the vtk Mesh as a vtk file  (default=off) | 


**clitkDicomWave2Text**

clitk 1.2

Extract data from a Dicom wave file to a text file

Usage: clitk [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | config file | 
| -i | --InputFile=STRING | Dicom inputfile name | 
| -o | --OutputFile=STRING | Text outputfile name | 
| -v | --Verbose=INT | verbose | 


**clitkElastixTransformToMatrix**

clitkElastixTransformToMatrix 1.0

Usage: clitkElastixTransformToMatrix [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input elastix filename | 
| -o | --output=STRING | Output matrix filename | 


**clitkExtractBones**

clitkExtractBones 1.0

Extract bony anatomy through thresholding and connected component labelling. 
Resample mask afterwards (interp= NN) to match another one (like)

Usage: clitkExtractBones [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -v | --verbose | Verbose  (default=off) | 
|   | --verboseStep | Verbose each step  (default=off) | 
| -w | --writeStep | Write image at each step  (default=off) | 
|   | --verboseOption | Display options values  (default=off) | 
|   | --verboseWarningOff | Do not display warning  (default=off) | 
|   | --verboseMemory | Display memory usage  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">I/O: | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -a | --afdb=STRING | Output Anatomical Feature DB (Carina position) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Smoothing (curvature anistropic diffusion): | 
|   | --smooth | smooth input image  (default=off) | 
|   | --spacing | Use image spacing  (default=off) | 
|   | --cond=DOUBLE | Conductance parameter  (default=`3.0') | 
|   | --time=DOUBLE | Time step (0.125 for 2D and 0.0625 for 3D)  (default=`0.0625') | 
|   | --iter=DOUBLE | Iterations  (default=`5') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Initial connected component labelling (CCL): | 
|   | --lower1=DOUBLE | Lower threshold for CCL  (default=`100') | 
|   | --upper1=DOUBLE | Upper threshold for CCL  (default=`1500') | 
|   | --minSize=INT | Minimal Component Size for CCL  (default=`100') | 
|   | --full | Use full connectivity (one side is enough)  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Neighborhood Connected Region Growing (RG): | 
|   | --lower2=DOUBLE | Lower threshold for RG  (default=`10') | 
|   | --upper2=DOUBLE | Upper threshold for RG  (default=`1500') | 
|   | --radius2=INT | Neighborhood radius  (default=`1') | 
|   | --sampleRate2=INT | Sample rate of label image for RG: number of voxels to skip between seeds  (default=`0') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Fill holes: | 
|   | --doNotFillHoles | Do not fill holes if set  (default=on) | 
| -d | --dir=INT | Directions (axes) to perform filling (defaults to 2,1,0) | 
|   | --noAutoCrop | If set : do no crop final mask to BoundingBox  (default=off) | 


**clitkExtractLung**

clitkExtractLungs 1.0

Segment lungs in CT image. Need 'patient' mask.

Usage: clitkExtractLungs [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -v | --verbose | Verbose  (default=off) | 
|   | --verboseStep | Verbose each step  (default=off) | 
| -w | --writeStep | Write image at each step  (default=off) | 
|   | --verboseOption | Display options values  (default=off) | 
|   | --verboseWarningOff | Do not display warning  (default=off) | 
|   | --verboseMemory | Display memory usage  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">I/O: | 
| -i | --input=STRING | Input CT image filename | 
| -a | --afdb=STRING | Output Anatomical Feature DB (Carina position)  (default=`default.afdb') | 
| -o | --output=STRING | Output lungs mask filename  (default=`lung.mhd') | 
| -t | --outputTrachea=STRING | Output trachea mask filename  (default=`trachea.mhd') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Step 1 : Air remove: | 
|   | --lower=INT | Initial lower threshold | 
|   | --upper=INT | Initial upper threshold  (default=`-300') | 
|   | --minSize=INT | Minimum component size in voxels  (default=`100') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Step 2 : find trachea: | 
|   | --type=INT | trachea finding algorithm (0: original; 1: LOLA11)  (default=`0') | 
|   | --skipslices=INT | 0: Number of slices to skip before searching seed  (default=`0') | 
|   | --upperThresholdForTrachea=DOUBLE | --upperThresholdForTrachea=DOUBLE0: Initial upper threshold for trachea  (default=`-900') | 
|   | --multiplierForTrachea=DOUBLE | --multiplierForTrachea=DOUBLE0: Multiplier for the region growing  (default=`5') | 
|   | --thresholdStepSizeForTrachea=INT | --thresholdStepSizeForTrachea=INT0: Threshold step size  (default=`64') | 
|   | --seed=FLOAT | 0,1: Index of the trachea seed point (in mm  NOT IN PIXELS) | 
|   | --doNotCheckTracheaVolume | 0,1: If set, do not check the trachea volume  (default=off) | 
|   | --verboseRG | 0,1: Verbose RegionGrowing  (default=off) | 
|   | --maxElongation=FLOAT | 1: Maximum allowed elongation of candidate regions for the trachea  (default=`0.5') | 
|   | --numSlices=INT | 1: Number of slices to search for trachea  (default=`50') | 
|   | --seedPreProcessingThreshold=INT | --seedPreProcessingThreshold=INT1: Threshold for the pre-processing step of the algorithm  (default=`-400') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Step 3 : auto extract lung: | 
|   | --bins=INT | Number of bins to use for the Otsu thresholding (default=`500') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Step 4 : remove trachea: | 
|   | --radius=INT | Radius for dilation  (default=`1') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Step 5 : [optional] openclose: | 
|   | --openclose | Perform an OpenClose operation  (default=off) | 
|   | --opencloseRadius=INT | OpenClose radius  (default=`1') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Step 6 : fill holes: | 
|   | --doNotFillHoles | Do not fill holes if set  (default=on) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Step 7 : lung separation (labelling): | 
|   | --doNotSeparateLungs | Do not separate lungs if set  (default=off) | 
|   | --removeSmallLabel | Remove small label (stomach?) before separation (default=on) | 
|   | --noAutoCrop | If set : do no crop final mask to BoundingBox  (default=off) | 


**clitkExtractPatient**

clitkExtractPatient 1.0

Input is binarized using initial thresholds, connected components are labeled 
(firstLabel). The air label (1) is removed. The remaining is binarized and 
relabeled, patient should now be the principal label (secondLabel). Two 
mechanismes are provided to influence the label images. Crop to reduce 
connectivity (image is restored to original size), eg for SBF. Decomposition 
through erosion and reconstruction through dilation (slow), eg for Pulmo 
bellows. Choose which labels to keep from second Label image. Final mask is 
cleaned by opening and closing.
The image is padded first with air. If lungs are touching the border (so the 
air), set openingRadius to 1 in order to have lungs segmented inside the 
patient

Usage: clitkExtractPatient [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -v | --verbose | Verbose  (default=off) | 
|   | --verboseStep | Verbose each step  (default=off) | 
| -w | --writeStep | Write image at each step  (default=off) | 
|   | --verboseOption | Display options values  (default=off) | 
|   | --verboseWarningOff | Do not display warning  (default=off) | 
|   | --verboseMemory | Display memory usage  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">I/O: | 
| -i | --input=STRING | Input image filename | 
| -a | --afdb=STRING | Output Anatomical Feature in a DB | 
| -o | --output=STRING | Output image filename | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Binarize: | 
|   | --lower=DOUBLE | Initial lower threshold | 
|   | --upper=DOUBLE | Initial upper threshold  (default=`-300') | 
|   | --openingRadius=INT | Radius for opening after threshold  (default=`0') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">First Label Image (air=1): | 
|   | --erode1 | Decompose through erosion  (default=off) | 
|   | --radius1=INT | Radius for erosion and dilation  (default=`1') | 
|   | --new1=INT | Number of new labels  (default=`1') | 
|   | --max1=INT | Max number of labels to consider  (default=`2') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Second Label Image (air=1): | 
|   | --erode2 | Decompose through erosion  (default=off) | 
|   | --radius2=INT | Radius for erosion and dilation  (default=`1') | 
|   | --new2=INT | Number of new labels  (default=`1') | 
|   | --max2=INT | Max number of labels to consider  (default=`2') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Labels to keep (air=1): | 
|   | --firstKeep=INT | First label to keep  (default=`1') | 
|   | --lastKeep=INT | Last label to keep  (default=`1') | 
|   | --remove=INT | Labels to remove | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Clean-up: | 
|   | --openClose | Perform morphological opening and closing with unit radius  (default=off) | 
|   | --noAutoCrop | If set : do no crop final mask to BoundingBox  (default=off) | 


**clitkExtrude**

clitkExtrude 1.0

Extrude an image i along the last+1 dimension repeating the image N times. So 
the output has 1 dimension more than the input.

Usage: clitkExtrude [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -N | --size=INT | Size in pixel of extrusion  (default=`1') | 
| -s | --spacing=DOUBLE | Spacing of the new dimension  (default=`1.0') | 
|   | --origin=DOUBLE | Origin of the new dimension  (default=`0.0') | 
| -l | --like=STRING | Size, spacing and origin like this image | 


**clitkFlipImage**

clitkFlipImage 1.0

Usage: clitkFlipImage [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -a | --axe=INT | Axe  (default=`0') | 
|   | --origin | Flip around origin  (default=off) | 


**clitkGammaIndex**

clitkGammaIndex 1.1

Compute the gamma index map for target dose map vs reference dose map. If 
verbose is given, the ratio of gamma>1 pixels of the total pixel in the image 
is computed. Absolute dose margin has priority over relative dose margin. The 
relative dose margin is relative to the maximum dose in the reference image.

clitkGammaIndex -v -i ref.mhd -j disp.mhd -o gamma.mhd -s 3 -r 0.04
this will compute the 3mm 4% gamma index between ref and disp.

Usage: clitkGammaIndex [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --reference=STRING | Reference dose map | 
| -j | --target=STRING | Target dose map | 
| -o | --output=STRING | Output image filename | 
| -s | --spatial-margin=DOUBLE | Spatial margin [mm] | 
| -r | --relative-dose-margin=DOUBLE | --relative-dose-margin=DOUBLEDose margin relative to max dose in reference [%] | 
| -d | --absolute-dose-margin=DOUBLE | --absolute-dose-margin=DOUBLEAbsolute dose margin [Gray] | 
| -x | --translation-x=DOUBLE | Target relative position x [mm]  (default=`0') | 
| -y | --translation-y=DOUBLE | Target relative position y [mm]  (default=`0') | 
| -z | --translation-z=DOUBLE | Target relative position z [mm]  (default=`0') | 


**clitkGateSimulation2Dicom**

clitkGateSimulation2Dicom 1.0

Usage: clitkGateSimulation2Dicom [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -d | --inputModelFilename=STRING | --inputModelFilename=STRINGInput dicom model | 
| -o | --outputFilename=STRING | Output dicom directory/filename | 


**clitkHistogramImage**

clitkHistogramImage 1.0

Save the histogram of the input image in the output text file

Usage: clitkHistogramImage [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output texte filename | 
| -s | --size=DOUBLE | size of the bin | 


**clitkImage2Dicom**

clitkImage2Dicom 1.0

Convert the 3D mhd input image (int image) into a 3D dicom (outputDcm) based on 
the dictionary of the dicoms in inputDcm. The number of slice between the input 
and the inputDcm could be different

Usage: clitkImage2Dicom [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -d | --inputDcm=STRING | Input dicom filename | 
| -o | --outputDcm=STRING | Output dicom folder | 
| -n | --nameDicom=STRING | Output dicom filename | 
| -u | --newUID | Write output with new UID  (default=off) | 
|   | --volume | Save a dicom volume  (default=off) | 
| -k | --key=STRING | Keys of tags to modify | 
| -t | --tag=STRING | Tags values | 


**clitkImage2DicomDose**

clitkImage2DicomDose 1.0

Convert mhd file into Dicom RTDose fil using a model

Usage: clitkImage2DicomDose [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -i | --input=STRING | inputfile name | 
| -m | --DicomInputFile=STRING | dicom model name | 
| -o | --output=STRING | output directory/filename | 
| -v | --verbose | Verbose  (default=off) | 


**clitkImageArithm**

clitkImageArithm 1.0

Perform an arithmetic operation (+-*/ ...) using two images or using an image 
and a scalar value.

Usage: clitkImageArithm [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -i | --input1=STRING | Input first image filename | 
| -j | --input2=STRING | Input second image filename | 
| -o | --output=STRING | Output image filename | 
| -s | --scalar=DOUBLE | Scalar value | 
| -t | --operation=INT | Type of operation : With another image : 0=add*, 1=multiply, 2=divide,3=max, 4=min, 5=absdiff, 6=squareddiff,  7=difference*, 8=relativ diff; For 'scalar' : 0=add*, 1=multiply*, 2=inverse,3=max, 4=min 5=absval 6=squareval7=ln 8=exp 9=sqrt 10=EPID 11=divide* 12=normalize (divide by max) 13=-ln(I/IO)**; * operations supported with vector fields as inputs. ** for fluence image, if pixel value == 0, consider value=0.5  (default=`0') | 
|   | --pixelValue=DOUBLE | Default value for NaN/Inf  (default=`0.0') | 
| -f | --setFloatOutput | Set output to float pixel type  (default=off) | 


**clitkImageConvert**

clitkImageConvert 1.0

Convert an image into another image.
Allow to change the file format and/or the pixel type. 
Known file formats 2D: jpeg png bmp tif mhd hdr vox dcm 
Known file formats 3D: mhd vox hdr dcm
Known file formats 4D: mhd 
Known images: 2D 3D or 4D, schar, uchar, short, ushort, int, float and double

If the -o option is not given, the last parameter of the command line is used 
as output.

Usage: clitkImageConvert [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -t | --type=STRING | Output type (float, ushort ...) | 
| -v | --verbose | Verbose  (default=off) | 
| -c | --compression | Compress output  (default=off) | 
|   | --vv | Read image as in vv and save transform in meta information  (default=off) | 
|   | --correct | Correct dicom with negative Z spacing  (default=off) | 
|   | --noniimeta | Multiply 1st and 2nd coordinate of Direction and Origin by -1  (default=off) | 


**clitkImageCreate**

clitkImageCreate 1.0

Create a new singled value filled image (pixeltype is float, use 
clitkImageConvert to change).

Usage: clitkImageCreate [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -o | --output=STRING | Output image filename | 
| -l | --like=STRING | Size/spacing like this other image | 
|   | --size=INT | Number of pixels of each coordinate | 
|   | --spacing=FLOAT | Spacing in mm between pixels | 
|   | --origin=DOUBLE | Origin in mm | 
|   | --transformMatrix=DOUBLE | Rotation matrix | 
|   | --value=FLOAT | Value for all voxels  (default=`0.0') | 
| -v | --verbose | Verbose  (default=off) | 


**clitkImageGradientMagnitude**

clitkImageGradientMagnitude 1.0

Compute the gradient magnitude of the input image

Usage: clitkImageGradientMagnitude [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -m | --mask=STRING | Mask input image filename | 
| -o | --output=STRING | Output image filename | 
| -n | --normalize | Normalize the output image between 0 and 1  (default=off) | 
| -g | --gaussian_filter | Gaussian filtering - default sigma value=1.0  (default=off) | 


**clitkImageInfo**

clitkImageInfo 1.0

Display information on images.

Usage: clitkImageInfo [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -l | --long=INT | Long line output (1 or 2)  (default=`1') | 
| -v | --verbose | Same as -l 1  (default=off) | 
| -n | --name | Display filename  (default=off) | 
| -m | --matrix | Display matrix  (default=off) | 


**clitkImageIntensityWindowing**

clitkImageIntensityWindowing 1.0

Usage: clitkImageIntensityWindowing [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -m | --mask=STRING | Mask input image filename | 


**clitkImageLaplacian**

clitkImageLaplacian 1.0

Compute the laplacian of the input image

Usage: clitkImageLaplacian [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -m | --mask=STRING | Mask input image filename | 
| -o | --output=STRING | Output image filename | 
| -n | --normalize | Normalize the output image between 0 and 1  (default=off) | 
| -g | --gaussian_filter | Gaussian filtering - default sigma value=1.0  (default=on) | 


**clitkImageMoment**

clitkImageMoment 1.0

Calculate the image moment: the center of gravity

Usage: clitkImageMoment [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -c | --center | Compute center of gravity  (default=on) | 
| -s | --second | Compute second order moments  (default=off) | 
| -a | --axes | Compute the principal axes  (default=off) | 


**clitkImageStatistics**

clitkImageStatistics 2.0

Compute statistics on an image, or on part of an image specified by a mask and 
label(s). The tool also supports multichannel images, which is useful, e.g., 
for vector fields. All channels are processed (separately) by default, but only 
one channel may be chosen.

Usage: clitkImageStatistics [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -c | --channel=INT | Image channel to be used in statistics (-1 to process all channels)  (default=`-1') | 
| -m | --mask=STRING | Mask image filename (uchar) | 
| -l | --label=INT | Label(s) in the mask image to consider  (default=`1') | 
|   | --localize | With verbose, write the index coordinates of the max and min  (default=off) | 
|   | --histogram=STRING | Compute histogram, allows median calculation | 
|   | --dvhistogram=STRING | Compute dose volume histogram | 
|   | --bins=INT | Number of histogram bins  (default=`100') | 
|   | --lower=DOUBLE | Lower histogram bound  (default=`-1000') | 
|   | --upper=DOUBLE | Upper histogram bound  (default=`1000') | 
| -r | --allow_resize | Resize mask if different from input  (default=off) | 


**clitkImageToVectorImage**

clitkImageToVectorImage 1.0

Compose 3 images to 1 vector image

Usage: clitkImageToVectorImage [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -o | --output=STRING | Output image filename | 


**clitkImageUncertainty**

clitk Compute the uncertainty of simulation outputs using Chetty's history-by-history's method

Usage: clitk [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -i | --input=STRING | Input image filename | 
| -s | --inputSquared=STRING | Input squared image filename | 
| -o | --output=STRING | Output image filename | 
| -n | --NumberOfEvents=INT | Number of events | 
| -v | --verbose | Verbose  (default=off) | 
| -a | --absolute | Absolute instead of the relative  (default=off) | 
| -d | --default=DOUBLE | Default when input is 0.  (default=`1.') | 


**clitkInvertVF**

clitk Read a DVF and invert it using a linear splat to the target, or by subsumpling the input grid and matching it to the output grid

Usage: clitk [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --threads=INT | Number of threads (default=min(8, #CPU)) | 
| -i | --input=STRING | Input VF filename | 
| -o | --output=STRING | Output VF filename | 
| -t | --type=INT | Type of filter: 0=clitk (fast forward splat using linear kernels), 1=clitk (like 0, but input images are B-Spline coefficients), 2= itk (grid subsumpling with controllable precision)  (default=`0') | 
|   | --threadSafe | Clitk: use thread safe algorithm  (default=off) | 
| -p | --pad=DOUBLE | Clitk: edge padding value (1 or N number of values!, defautls to zeros) | 
| -s | --sampling=INT | Itk: subsampling factor  (default=`20') | 
| -l | --like=STRING | Image whose grid (spacing and size) will be used for output | 


**clitkJacobianImage**

clitk Read a DVF and calculate the corresponding jacobian image.

Usage: clitk [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input VF filename | 
| -o | --output=STRING | Output VF filename | 


**clitkMaskLandmarks**

clitkMaskLandmarks 1.0

Remove landmarks outside of a given mask also remove their corresponding points 
if multiple input are given.

Usage: clitkMaskLandmarks [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input landmarks filename | 
| -m | --mask=STRING | Input mask | 
| -o | --output=STRING | Output landmarks filename | 


**clitkMaskOfIntegratedIntensity**

clitkMaskOfIntegratedIntensity 1.0

Compute a mask that represent X% of the total pixels values

Usage: clitkMaskOfIntegratedIntensity [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -p | --percentage=DOUBLE | Percentage of total pixels values (in %) | 


**clitkMatrixInverse**

clitkMatrixInverse 1.0

Usage: clitkMatrixInverse [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -i | --input=STRING | Input matrix filename | 
| -o | --output=STRING | Output matrix filename | 


**clitkMatrixMultiply**

clitkMatrixMultiply 1.0

Multiply two 4x4 matrices. In matrice notations, does input2*input

Usage: clitkMatrixMultiply [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -i | --input1=STRING | Input matrix filename | 
| -j | --input2=STRING | Input matrix filename | 
| -o | --output=STRING | Output matrix filename | 


**clitkMatrixToElastixTransform**

clitkMatrixToElastixTransform 1.0

Usage: clitkMatrixToElastixTransform [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input matrix filename | 
| -o | --output=STRING | Output elastix filename | 
|   | --center=FLOAT | Rotation center | 


**clitkMatrixTransformToVF**

clitkMatrixTransformToVF 1.0

Convert a matrix to a DVF with given properties

Usage: clitkMatrixTransformToVF [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --matrix=STRING | Input matrix filename | 
| -o | --output=STRING | Output image filename | 
| -d | --dim=INT | Dimension  (default=`3') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Output Image Properties: | 
|   | --like=STRING | Make output like this image | 
|   | --origin=DOUBLE | Origin for the output image  (default=`0.0') | 
|   | --size=INT | Size for the output image  (default=`100') | 
|   | --spacing=DOUBLE | Spacing for the output image  (default=`1.0') | 


**clitkMedianImageFilter**

clitkMedianImageFilter 1.0

Usage: clitkMedianImageFilter [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -r | --radius=INT | Radius in each Direction  (default=`1') | 


**clitkMedianTemporalDimension**

clitkMedianTemporalDimension 1.0

Average the last dimension to an (n-1)D image. Input is either nD or multiple 
(n-1)D image or DVF

Usage: clitkMedianTemporalDimension [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 


**clitkMergeAsciiDoseActor**

clitk Add values from two ASCII files (must contains list of numbers)

Usage: clitk [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -i | --input1=STRING | Input ASCII file | 
| -j | --input2=STRING | Input ASCII file | 
| -o | --output=STRING | Output ASCII file | 


**clitkMergeRootFiles**

clitkMergeRootFiles 1.0

Merge several ROOT files

Usage: clitkMergeRootFiles [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -i | --input=STRING | Input ROOT filenames | 
| -o | --output=STRING | Output ROOT filename | 
| -f | --fastmerge | Fast merge | 
| -v | --verbose=INT | Verbose level | 


**clitkMergeSequence**

clitkMergeSequence 1.0

Read a series of nD images (unnamed inputs) and merge them to a (n+1)D image.
The spacing of the (n+1)th dimension is given by the option spacing.

Usage: clitkMergeSequence [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -s | --spacing=DOUBLE | Spacing for the new dimension  (default=`1') | 
| -o | --output=STRING | Output filename | 
| -v | --verbose | Verbose  (default=off) | 


**clitkMinMaxMask**

clitkMinMaxMask 1.0

ssdfgsfg ssdf g.

Usage: clitkMinMaxMask [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input1=STRING | Input mask image filename | 
| -j | --input2=STRING | Input mask  image filename | 


**clitkMIP**

clitkMIP 1.0

Usage: clitkMIP [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -d | --dimension=INT | Dimension along which to project | 


**clitkMirrorPadImage**

clitkMirrorPadImage 1.0

Usage: clitkMirrorPadImage [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -m | --mirror=INT | Mirror the entire image along given axis | 
| -p | --pad=INT | Mirror pad with given size | 
| -l | --lower | Pad on lower end (else upper)  (default=off) | 
|   | --origin | Set new origin to 0  (default=off) | 


**clitkMorphoMath**

clitk Perform standard morphological operations

Usage: clitk [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Input: | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Processing Parameters: | 
| -t | --type=INT | 0=Erode, 1=Dilate, 2=Close (erode(dilate(x))), 3=Open (dilate(erode(x))), 4=CondErode, 5=CondDilate  (default=`0') | 
|   | --fg=FLOAT | Foreground value  (default=`1') | 
|   | --bg=FLOAT | Background value (0,1,3: filling value)  (default=`0') | 
| -b | --bound | 0-1: Set borders to foreground/ 2:safe borders   (default=off) | 
| -r | --radius=INT | Use binary ball element with given radius  (default=`1') | 
| -m | --radiusInMM=DOUBLE | Use binary ball element with given radius in mm (rounded to nearest voxel value), you can give one radius by dimension  (default=`1') | 
|   | --extend | Extend the image size according to radius  (default=off) | 


**clitkMotionMask**

clitkMotionMask 1.0

From an input CT image (HU), extract feature images (air, ribs and lungs) and 
calculate the motion mask using levelsets, Vandemeulebroucke2010, ICCR. More 
elaborate methods for extracting the feature images have been incude in 
clitkExtract*, with *=Lungs,Bones,Patient.

Usage: clitkMotionMask [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">I/O: | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -m | --monitor=STRING | Monitoring image for levelsets | 
|   | --spacing=DOUBLE | Low dimensional spacing to perform initial level set steps  (default=`4') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Feature Images (feature=1,rest=0). Set them or extract them from the input: | 
|   | --featureAir=STRING | Input feature image | 
|   | --lowerThresholdAir=DOUBLE | --lowerThresholdAir=DOUBLELower threshold for air feature image extraction  (default=`-10000') | 
|   | --upperThresholdAir=DOUBLE | --upperThresholdAir=DOUBLEUpper threshold for air feature image extraction  (default=`-800') | 
|   | --pad | Make a border of air around the image (for cropped images)  (default=off) | 
|   | --featureBones=STRING | Input feature image | 
|   | --lowerThresholdBones=DOUBLE | --lowerThresholdBones=DOUBLELower threshold for bones feature image extraction  (default=`100') | 
|   | --upperThresholdBones=DOUBLE | --upperThresholdBones=DOUBLEUpper threshold for bones feature image extraction  (default=`1000') | 
|   | --featureLungs=STRING | Input feature image | 
|   | --lowerThresholdLungs=DOUBLE | --lowerThresholdLungs=DOUBLELower threshold for lungs feature image extraction  (default=`-950') | 
|   | --upperThresholdLungs=DOUBLE | --upperThresholdLungs=DOUBLEUpper threshold for lungs feature image extraction  (default=`-600') | 
|   | --writeFeature=STRING | Write the combined feature image | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Ellipsoide initialization: | 
|   | --ellips=STRING | Input ellipsoide image (=1, at half resolution) | 
|   | --writeEllips=STRING | Write the initial ellipsoide image | 
|   | --writeDistMap=STRING | Write the distance map | 
|   | --ellipseAutoDetect | Auto-detect offset and axes of initial ellipse  (default=off) | 
|   | --offset=DOUBLE | Offset for ellips center from body center of gravity (default= 0,-50,0 mm) | 
|   | --axes=DOUBLE | Half axes of the ellips (default= 100,30,150) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Ellipsoide growing: | 
|   | --grownEllips=STRING | Input grown ellips image (=1, at half resolution) | 
|   | --offsetDetect=DOUBLE | Offset of detection point from abdomen (default= 0,-10,0 mm) | 
|   | --detectionPairs=STRING | Additional images to detect the abdomen (eg end-inhalation frame). The most anterior point will be retained. | 
|   | --detectionPoint=DOUBLE | Physical coordinates of the detection point from abdomen (default= 0,-10,0 mm) | 
|   | --curve1=DOUBLE | Curvature for this levelset  (default=`35.0') | 
|   | --maxRMS1=DOUBLE | Tolerance for this levelset  (default=`0.001') | 
|   | --iter1=INT | Iterations performed between monitoring  (default=`50') | 
|   | --writeGrownEllips=STRING | Write the grown ellipsoide image | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Filling the bones image: | 
|   | --filledRibs=STRING | Input filled rib image (=1, at half resolution) | 
|   | --fillingLevel=DOUBLE | Minimum lung fill level: [0,100] %  (default=`98.0') | 
|   | --curve2=DOUBLE | Curvature for this levelset  (default=`30.0') | 
|   | --maxRMS2=DOUBLE | Tolerance for this levelset  (default=`0.001') | 
|   | --iter2=INT | Iterations performed between monitoring  (default=`50') | 
|   | --writeFilledRibs=STRING | Write the filled ribs image image | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Collapsing to the lung image: | 
|   | --curve3=DOUBLE | Curvature for this levelset  (default=`30.0') | 
|   | --prop3=DOUBLE | Propagation for this levelset  (default=`0') | 
|   | --maxRMS3=DOUBLE | Tolerance for this levelset  (default=`0.001') | 
|   | --iter3=INT | Iterations performed between monitoring  (default=`20') | 
|   | --maxIter3=INT | Iterations performed between monitoring  (default=`500') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Clean-up: | 
|   | --openClose | Perform morphological opening and closing with unit radius  (default=on) | 


**clitkNormalizeImageFilter**

clitkNormalizeImageFilter 1.0

Usage: clitkNormalizeImageFilter [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -m | --mask=STRING | Mask input image filename | 
| -n | --total_normalisation | Normalise such as the total is = 1.0  (default=off) | 


**clitkNVectorImageTo4DImage**

clitkNVectorImageTo4DImage 1.0

Convert all pixel channels of the image into a 4D image

Usage: clitkNVectorImageTo4DImage [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 


**clitkPadImage**

clitkPadImage 1.0

Pad an image according to a given extends or like another image

Usage: clitkPadImage [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">I/O: | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Used determined padding: | 
| -l | --lower=INT | Size of the lower bound padding (multiple values=number of image dimension) | 
| -u | --upper=INT | Size of the upper bound padding (multiple values=number of image dimension) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Pad like another image: | 
|   | --like=STRING | Pad like this image (must have the same spacing and bounding box must be larger) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Extra parameters: | 
|   | --value=FLOAT | Value to be set in padded area  (default=`0') | 


**clitkPartitionEnergyWindowDicom**

clitkPartitionEnergyWindowDicom 1.0

From one Dicom with different energy windows (eg with SPECT), the tool create 
new dicoms for each energy window and copy the correct dicom tag

Usage: clitkPartitionEnergyWindowDicom [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input Dicom image filename | 
| -o | --output=STRING | Output dicom directory | 


**clitkProfileImage**

clitkProfileImage 1.0

Save the profile between 2 points of the input image in the output text file

Usage: clitkProfileImage [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output texte filename | 
| -f | --point1=DOUBLE | First point (vox) | 
| -s | --point2=DOUBLE | Second point (vox) | 


**clitkRegionGrowing**

clitkRegionGrowing 1.0

Region growing from a seed point using various types of conditions to control 
the growing

Usage: clitkRegionGrowing [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">I/O: | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Parameters: | 
| -t | --type=INT | Region growing filter type: 0=threshold , 1=neighborhood-threshold , 2=confidence , 3=locally-adaptive-threshold, 4=explosion-controlled-threshold  (default=`0') | 
| -l | --lower=DOUBLE | Lower threshold value  (default=`310') | 
| -u | --upper=DOUBLE | Upper threshold value  (default=`4000') | 
| -s | --seed=INT | Seed index postion (in voxels)  (default=`0') | 
|   | --seedRadius=INT | Radius used for seed dilatation(in voxel)  (default=`0') | 
| -p | --pad=DOUBLE | The replace padding value  (default=`1.0') | 
| -r | --radius=INT | 1-3: The radius of the neighborhood  (default=`1') | 
|   | --iter=INT | 2: Iterations  (default=`5') | 
| -m | --multiplier=DOUBLE | 2-4: (2-3) accept if within mean+-mutiplier*SD, (4) explosion if size increases multiplier times  (default=`2.0') | 
|   | --maxSD=DOUBLE | 3: Limit to SD | 
|   | --adaptLower | 3,4: (locally) adapt lower thresholding  (default=off) | 
|   | --adaptUpper | 3,4: (locally) adapt upper thresholding  (default=off) | 
|   | --maxUpper=DOUBLE | 4: Maximum upper threshold value  (default=`2000') | 
|   | --minLower=DOUBLE | 4: Minimum lower threshold value  (default=`-1000') | 
|   | --step=DOUBLE | 4: Threshold step size  (default=`64.0') | 
|   | --minStep=DOUBLE | 4: Minimum threshold step size  (default=`1') | 
|   | --full | 4: use full connectivity (not implemented yet)  (default=off) | 


**clitkRTStructStatistics**

clitkRTStructStatistics 2.0

Find the centroid (in mm) and roundness of a binarized image.

Usage: clitkRTStructStatistics [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input image filename (mask) | 


**clitkScintivolStats**

clitkScintivolStats 1.0

Convert an image into another image.
Allow to change the file format and/or the pixel type. 
Known file formats 2D: jpeg png bmp tif mhd hdr vox dcm 
Known file formats 3D: mhd vox hdr dcm
Known file formats 4D: mhd 
Known images: 2D 3D or 4D, schar, uchar, short, ushort, int, float and double

If the -o option is not given, the last parameter of the command line is used 
as output.

Usage: clitkScintivolStats [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -o | --output=STRING | Output .csv filename | 
|   | --append | Append results to .csv file (eg: for scatter)  (default=off) | 
|   | --dynamic1=STRING | Dynamique 1 filename (geometrical mean) | 
|   | --frameDurationDynamic1=DOUBLE | --frameDurationDynamic1=DOUBLEFrame duration for dynamique 1 | 
|   | --heartMask=STRING | Heart mask filename | 
|   | --liverMask=STRING | Liver mask filename | 
|   | --dynamic2=STRING | Dynamique 2 filename (geometrical mean) | 
|   | --frameDurationDynamic2=DOUBLE | --frameDurationDynamic2=DOUBLEFrame duration for dynamique 2 | 
|   | --acquisitionTimeDynamic2=DOUBLE | --acquisitionTimeDynamic2=DOUBLEStart acquisition time of the dynamic2 after dynamic1 | 
|   | --parenchymaMask=STRING | Parenchyma mask filename | 
|   | --tomo=STRING | Tomo image filename | 
|   | --acquisitionTimeTomo=DOUBLE | --acquisitionTimeTomo=DOUBLEAcquisition time for tomo | 
|   | --totalLiverMask=STRING | Total liver mask filename | 
|   | --remnantLiverMask=STRING | Remnant liver mask filename | 


**clitkSetBackground**

clitkSetBackground 1.0

Usage: clitkSetBackground [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -m | --mask=STRING | Mask image filename | 
| -p | --outsideValue=DOUBLE | Outside value  (default=`0.0') | 
| -l | --maskValue=DOUBLE | Mask value  (default=`0.0') | 
|   | --fg | Foreground mode  (default=off) | 


**clitkSpect2Dicom**

clitkSpect2Dicom 1.0

Convert the 3D mhd input SPECT image (int image) into a 3D dicom (outputDcm) 
based on the dictionary of the dicoms in inputDcm.

Usage: clitkSpect2Dicom [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -d | --inputDcm=STRING | Input dicom filename | 
| -o | --outputDcm=STRING | Output dicom filename | 


**clitkSplitImage**

clitkSplitImage 1.0

Split an image into n images along a dimension

Usage: clitkSplitImage [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image base filename | 
| -v | --verbose | Verbose  (default=off) | 
| -d | --dimension=INT | Dimension to split on | 
| -p | --png | Png file format  (default=off) | 
| -w | --window=DOUBLE | Window | 
| -l | --level=DOUBLE | Level | 


**clitkSum**

clitkSum 1.0

Usage: clitkSum [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -d | --dimension=INT | Dimension along which to sum | 


**clitkSUVPeak**

clitkSUVPeak 2.0

This tool Compute the SUV Peak.
The output is the position of the SUV Peak (in mm) and its value.
The filter use a normalized sphere of 1 cc with 1 for voxel inside, 0 for 
outside.
And for voxels that intercepted the sphere, it computes the intersection volume 
using Monte Carlo simulation.

Usage: clitkSUVPeak [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input first image filename | 
| -m | --mask=STRING | Mask image filename (uchar) | 
| -r | --allow_resize | Resize mask if different from input  (default=off) | 
|   | --volume=DOUBLE | Volume of the filter in cc  (default=`1') | 


**clitkTransformLandmarks**

clitkTransformLandmarks 1.0

Trasnform a set of landmarks with the given 4x4 matrix. Output to stdout.

Usage: clitkTransformLandmarks [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input landmarks filename | 
| -m | --matrix=STRING | Input 4x4 matrix filename ('.mat' file) | 
| -s | --spacing=DOUBLE | If given, applies the given spacing (x,y,z) to the input points.  (default=`1') | 
| -o | --output=STRING | Output landmarks filename | 
| -t | --type=STRING | Landmarks type ('pts' for Jef; 'txt' for VV ; 'vtk' for vtk meshes)  (default=`txt') | 


**clitkUnsharpMask**

clitkUnsharpMask 1.0

Image sharpening technique. It computes Image - blurred(Image). Here 
blurred(Image) is the output of a gaussian filtering with a sigma measured in 
world coordinates

Usage: clitkUnsharpMask [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -s | --sigma=DOUBLE | Sigma of the gaussian | 


**clitkUpdateVRTagDicom**

clitkUpdateVRTagDicom 1.0

Fix VR Dicom tag mistakes with gdcm

Usage: clitkUpdateVRTagDicom [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input Dicom image filename | 
| -o | --output=STRING | Output dicom filename | 
| -m | --model=STRING | Model dicom filename | 


**clitkVectorArithm**

clitkVectorArithm 1.0

Perform an arithmetic operation (+-*/ ...) using two images or using an image 
and a scalar value.

Usage: clitkVectorArithm [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|   | --imagetypes | Display allowed image types  (default=off) | 
| -i | --input1=STRING | Input first image filename | 
| -j | --input2=STRING | Input second image filename | 
| -o | --output=STRING | Output image filename | 
| -s | --scalar=DOUBLE | Scalar value | 
| -t | --operation=INT | Type of operation : With another image : 0=add, 1=multiply, 2=dotproduct, 7=difference, 9=crossproduct; For 'scalar' : 0=add, 1=multiply, 5=absval (magnitude), 6=squared magnitude, 11=divide, 12=normalize  (default=`0') | 
|   | --pixelValue=DOUBLE | Default value for NaN/Inf  (default=`0.0') | 
| -f | --setFloatOutput | Set output to float pixel type  (default=off) | 


**clitkVectorImageToImage**

clitkVectorImageToImage 1.0

Extract a component image from a vector image

Usage: clitkVectorImageToImage [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -c | --componentIndex=INT | Component index to extract  (default=`0') | 


**clitkVFConvert**

clitkVFConvert 1.0

Convert file format

Usage: clitkVFConvert [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 


**clitkVFInterpolate**

clitkImageInterpolate 1.0

Interpolate an image. You can specify the interpolation, you can apply a 
Gaussian filter before.

Usage: clitkImageInterpolate [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -i | --input1=STRING | Input image filename | 
| -j | --input2=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -d | --distance=FLOAT | Distance (d in [0,1]) | 
| -v | --verbose | Verbose  (default=off) | 
|   | --interp=STRING | Interpolation type: {nn, linear}  (default=`nn') | 


**clitkVFResample**

clitkImageResample 1.0

Resample an image. You can specify the interpolation, you can apply a Gaussian 
filter before.

Usage: clitkImageResample [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filename | 
| -v | --verbose | Verbose  (default=off) | 
|   | --interp=STRING | Interpolation type: {nn, linear, bspline, blut}  (default=`nn') | 
| -b | --order=INT | BSpline ordre (range 0-5)  (default=`3') | 
| -s | --sampling=INT | BLUT sampling value  (default=`30') | 
|   | --size=INT | Number of pixels of each coordonate  (default=`0') | 
|   | --spacing=FLOAT | Spacing in mm between pixels  (default=`0.0') | 
| -g | --gauss=FLOAT | Apply Gaussian before (sigma in mm) ; no Gaussian if not given  (default=`1.0') | 
| -d | --default=FLOAT | Default pixel value  (default=`0.0') | 


**clitkWarpImage**

clitkWarpImage 1.0

Warp an image. Output will have the extent (origin+ spacing*size) of the input.

Usage: clitkWarpImage [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">I/O: | 
| -i | --input=STRING | Input image filename | 
| -o | --output=STRING | Output image filenameGroup: DVFoptionan option of this group is required | 
|   | --vf=STRING | Vector field filename | 
|   | --coeff=STRING | B-Spline coefficients filename | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Options: | 
| -f | --forward | Use forward warping (only linear interp)  (default=off) | 
| -s | --spacing=INT | The output spacing: 0=like the VF, 1= like the input image (interpolation of the VF prior to warp)  (default=`0') | 
|   | --pad=FLOAT | Edge padding value  (default=`0.0') | 
|  |  | <style="&quot; &amp; quot;text-align:center&amp; quot; &quot;">Interpolation: for image intensity and prior resampling of DVF: | 
|   | --interp=INT | Interpolation: 0=NN, 1=Linear, 2=BSpline, 3=BLUT  (default=`1') | 
|   | --interpOrder=INT | Order if BLUT or BSpline (0-5)  (default=`3') | 
|   | --interpSF=INT | Sampling factor if BLUT  (default=`20') | 
|   | --interpVF=INT | Interpolation: 0=NN, 1=Linear, 2=BSpline, 3=BLUT  (default=`1') | 
|   | --interpVFOrder=INT | Order if BLUT or BSpline (0-5)  (default=`3') | 
|   | --interpVFSF=INT | Sampling factor if BLUT  (default=`20') | 


**clitkWriteDicomSeries**

clitkWriteDicomSeries 1.0

Usage: clitkWriteDicomSeries [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -v | --verbose | Verbose  (default=off) | 
| -i | --input=STRING | Input image filename | 
| -d | --inputDir=STRING | Input dicom directory | 
| -o | --outputDir=STRING | Output dicom directory | 
| -k | --key=STRING | Keys of tags to modify  (default=`0008|103e') | 
| -t | --tag=STRING | Tags values  (default=`MIDPOSITION') | 
| -e | --newSeriesUID | Write the series with a new series UID (ignored if given in key/tag option)  (default=off) | 
| -u | --newStudyUID | Write the series with a new study UID (ignored if given in key/tag option; forces newSeriesUID = true)  (default=off) | 
| -s | --useSizeAsReference | Use the size of the dicom image as reference for an occasional resampling  (default=off) | 
| -p | --preserve | Preserve private tags and keep original UID (eg: Xio)  (default=off) | 


**clitkZeroVF**

clitk Read a vector fields (.mhd, .vf, ..) and zero it

Usage: clitk [OPTIONS]... [FILES]...

| | | |
| - | - | - |
| -h | --help | Print help and exit | 
| -V | --version | Print version and exit | 
|   | --config=STRING | Config file | 
| -i | --input=STRING | Input VF filename | 
| -o | --output=STRING | Output VF filename | 
| -v | --verbose | Verbose  (default=off) | 

