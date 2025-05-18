# SingScript.sg

## About this project...
This is a font modelled after my handwriting. It is supposed to contain special mathematical symbols for use in Math / Science applications, but that part has not been made yet. All 05524F software uses this font.

## Built with...
- FontForge (font editor)
- MinGW (compile C++ script)
- ChatGPT (generate Python script from pseudocode)

## Prerequisites
Any computer system supporting the .otf file format can use the SingScript font.

If you are interested in the kerning scripts, pls refer to the "Using the software" section below.

## Installation
Download the file "SingScript.sg.otf" and install the font. On Windows, this can be done by opening the font file and clicking "install" in the top left of the window. On other platforms, you can find tutorials online.

If your device does not support .otf, you can download FontForge and use the "SingScript Core.sfd" file to generate the font in another format.

## Using the software...
To use the font, simply enter its name "SingScript.sg" in place of "Arial" or "Times New Roman". Note that the font name is "SingScript.sg regular" in Adobe software.

You may find the template "SingScript template.pdf" helpful if you want to make your own font modelled after your handwriting. You can print out the template, write the letters, scan them, then trace them in FontForge.

The kerning scripts are also provided in this repository to automatically kern any font. If it does not work perfectly, complain to me and I will make a generalised version. To use the scripts, first run the Python script "getglyphspython.txt" in FontForge to extract the glyphs. Then compile using any C++ compiler and run "kerningcalculator.cpp" to generate the kerning data. The kerning data can be reuploaded into the font by running the Python script "applykerningpython.txt". Remember to replace the "path\\to\\root" part of the file paths in the scripts with the actual path on your computer.

Since coding in Python is extremely annoying, I wrote C++ style pseudocode and used ChatGPT to translate it to Python. However, no generative AI is used for kerningcalculator.cpp, so the logic should be fine. The files "applykerningpseudo.txt" and "getglyphspseudo.txt" contain the pseudocode.

## What to expect...
- [x] SingScript Core (ASCII, common characters, whitespace)
- [ ] SingScript 0 (celebrating Singaporean culture, 05524F icons and emojis)
- [ ] SingScript 1 (exponents, subscripts, logic, Mathematics, Science)
- [ ] SingScript 2 (Optical Answer Sheet, tables)
- [ ] SingScript 3 (non Unicode compliant full set of exponents and subscripts, this interferes with display of phonetic symbols)
- [ ] SingScript 4 (logic, Mathematics, Science - extension 1)
- [ ] SingScript 5 (logic, Mathematics, Science - extension 2)
- [ ] SingScript 6 (logic, Mathematics, Science - extension 3)

## Contributing...
The best way to contribute is to complain. To complain refers to informing me of a bug or suggesting a new feature to be included to the software. Pls refer to the "Contact me" section below.

If I perceive your suggested feature to be beyond the scope of the software, I will not approve it. However, you can still download the code and build your own feature. As long as you keep the .sg in your project name, I am ok with it.

## Who contributed the most...
student 05524F (InfinityIntegral)

(that is me)

(the index number comes from my index number in the Primary School Leaving Examinations 2021)

## License info...
This software is distributed under the MIT License. For more information, pls see LICENSE.txt

## Contact me...
Choose one of the following
- email to infinityintegral333@gmail.com
- message @05524F.sg on Instagram

## Made possible by...
- the README format is inspired by Best-README-Template, which can be found at https://github.com/othneildrew/Best-README-Template
- FontForge is used to make the font, their official site is https://fontforge.org
- MinGW is used to compile the C++ code, you can download a copy at https://mingw-w64.org
- ChatGPT is used to generate Python scripts from C++ style pseudocode
- the license terms of the MIT license are copied from the website of the Open Source Initiative which can be accessed at https://opensource.org
- the version control used is Git, their official website is https://git-scm.com
- the backup method used is GitHub, their official website is https://github.com
- All 05524F software is powered by Singapore, this refers to that student 05524F (InfinityIntegral) learnt programming, mathematics, logic, and self-study skills in Singapore.

## Am I blur...
No.
