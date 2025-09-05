## Project Title
LEGOnardo DaVinci

## Team Members
Joe Robertazzi & Gui Suranyi

## Project Description
Have you ever tried using Cardinal Print to print a final review sheet and have it fail on you? Do you HATE paying for 
paper and ink every time you want a cute cat picture? Well, if so, LEGOnardo DaVinci may be the product for you! 

LEGOnardo DaVinci is a printer that creates scaled-down images using LEGO as ink. First, users can upload their images to
our printing library via laptop (anything downloaded on your computer will convert and add to the library) via Python script.
Then, once uploaded, the program and printer are all run on a MangoPi which provides the user with an interface to select 
which image they want to print, the scale at which they want it to print, and the color map they want to use for said print.
Once the print is in the queue, a live real-time print is displayed on the screen as well, to inform the user of the printer's 
progress percentage along with how much ink has been used and how much is left. Once it finishes, you have your completed
image!

## Member Contribution
Joe - Wrote software for processing input images, scaling them up and down, cropping them, and changing their color maps all
      on the MangoPi (only Python/non-baremetal aspect being the image conversion from downloads on the laptop to bitmaps.c file).
      Additionally, wrote the user interface for the printer including the title splash screen, a menu consisting of loaded images
      on the printer, and interface while printing.
      
Gui - Built and designed all aspects of the printer itself including the vacuum mechanism, LEGO feeders/storage containers, and 
      CNC machine which was repurposed for the project. Additionally, wired all of the mechanisms together and wrote drivers for 
      moving stepper motors on the MangoPi.

## References
Inspiration: https://www.youtube.com/watch?v=Y4neo4fRw2M

All code was original work by us.

## Self-Evaluation
Our team was able to fully execute our project, and we're both very satisfied with how it turned out in the end. Not only were we 
able to meet the expectations that we set for ourselves in our proposal, but I think we surpassed them by printing out a full image
(Yoshi) and creating a full user interface for the printer. Both of us put in an immense amount of effort into this project, and
the feeling of everything working together in the end makes it all worth it. We also had our heroic moments, whether
that be successfully being able to pick up a LEGO piece with a vacuum for the first time or getting loaded images to change to the
right color.

## Photos
All media is within the 'pics_n_vids' folder of this repo! 
(or a much easier way is the Google Drive link: https://drive.google.com/drive/folders/1K85u9TBoneZYjCPITP94Ezn1deumaIid?usp=share_link)
