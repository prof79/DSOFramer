# DSOFramer ActiveX Control

This is slight code quality and compatibility rework of Microsoft's Office-embedding sample component "DSOFramer" (KB311765) based on [shines77](https://github.com/shines77/DsoFramer)'s work.

Project and sources are tested and compilable with Visual Studio 2022 (folder `Sources_VC2022`). Recommendations of VC2022 and Copilot have been followed to get rid of code quality warnings as possible. Also, further formatting for better readibility is done. Slight refactoring steps have begun like replacing `wsprintf()` with secure `StringCbPrintf()`. I aspire to attempt a full Unicode conversion.

The original DSOFramer has since been deprecated and retired by Microsoft and is no longer available for download. Take a look at the `Microsoft` folder what could have been recovered/preserved from original sources.

It is still, and most interestingly, despite all shortcomings there may be, working with Microsoft Office 365 (including Office LTSC 2024) - but **be aware** that your embedding application has to be **DPI-aware** lest Office applications display in a separate window:

* <https://stackoverflow.com/questions/72477616/office-365-word-cant-be-embed-in-place-inside-windows-native-application>
* <https://learn.microsoft.com/en-us/windows/win32/hidpi/setting-the-default-dpi-awareness-for-a-process>
* <https://stackoverflow.com/questions/4075802/creating-a-dpi-aware-application>

For a quick fix/proof-of-concept you can use [Resource Hacker](https://www.angusj.com/resourcehacker) to edit your executable's manifest resource according to the Microsoft Learn sample.

## Instructions

* See `Sources_VC2022` for the updated/work-in-progress sources.
* See `Sources` for the original Microsoft sources for comparison.
* See `Microsoft` for original binaries recovered before they were removed from the original pages.
* See `Microsoft\eula.txt` about what Microsoft originally said about licensing and liabilities.
