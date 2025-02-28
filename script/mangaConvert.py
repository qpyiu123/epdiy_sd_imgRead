import os
from PIL import Image

def convertImg(inFile, outFile):
    img = Image.open(inFile)
    if img.height>img.width:
        img = img.transpose(Image.ROTATE_90)
    img.save(outFile)




def main():
    inDir = input('InDir> ').replace('"','')
    outDir = input('OutDir>').replace('"','')
    os.makedirs(outDir, exist_ok=True)
    files = [i for i in os.listdir(inDir) if i[-4:]=='.png']
    for i in range(len(files)):
        convertImg(inDir+'/'+files[i], outDir+'/'+str(i)+".bmp")

if __name__ == '__main__':
    main()
