Welcome to the CSIRO Face Analysis SDK. Documentation for the SDK can
be found in doc/documentation.html.

All code in this SDK is provided according to the license found in
LICENSE.

If you use the CSIRO Face Analysis SDK in any publications, we ask
that you reference our works.

The face tracking component is based on the publication: 
J. Saragih, S. Lucey and J. Cohn, "Deformable Model Fitting by
Regularized Landmark Mean-Shift", IJCV 2011.

The expression transfer component is based on the publication:
J. Saragih, S. Lucey and J. Cohn, "Real-time Avatar Animation from a
Single Image", AFGR Workshop 2011.

If you use the SDK, we ask that you reference the following paper:
M. Cox, J. Nuevo, J. Saragih and S. Lucey, "CSIRO Face Analysis SDK",
AFGR 2013.

---

Here are the updates in the original code included in this fork of the project:

- The dependency of Qt in the qt-gui project was upgraded from Qt4 to Qt5.
- Some OpenCV missing includes were added (perhaps they are due to changes in version used).
- The type casts were made explicit to remove related warning messages.
- Some static array allocations were replaced with the operator `new[]`.
- The Windows/POSIX-specific function calls were enclosed in `#ifdefs` based on the operating system defined at compilation time, and the the best corresponding functions were used for Windows (for instance: `PathRemoveFileSpec` and `PathStripPath` respectively replaced `dirname` and `basename` in Windows).
- The class `ForkRunner` was completely replaced in Windows by the implementation proposed by Ben Howell [here](http://www.benhowell.net/guide/2015/03/16/porting-face-analysis-sdk-to-windows/). Although, [there is no direct equivalent of `fork` (used by the original class) on Windows](http://stackoverflow.com/a/9148072/2896619) and [`CreateProcess` does not have the same behaviour](http://stackoverflow.com/q/985281/2896619) (further tests required).
- The libraries `utilities`, `clmTracker` and `avatarAnim`, that were originally created as shared libraries, were transformed into static libraries only for the Windows configuration. This was just simpler to do at this time, because the use of the `__declspec(dllexport)` directive (to export the functions/classes to be usable through a DLL) was causing problems related to third part objects not being equally exported (for instance, in some classes/functions, the compiler was producing messages like: "warning C4251: class 'cv::Mat' needs to have dll-interface to be used by clients of class <the class in which cv::Mat was used>").