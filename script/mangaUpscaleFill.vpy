import vapoursynth as vs
core = vs.core
import os
import mvsfunc as mvs

target = [1600, 2560] # [w, h]

inDir = r'D:\user\Desktop\fkn9y245_files\外天楼'
outDir = r'D:\user\Desktop\script\out\外天楼'

def convert(filename):
    src = core.imwri.Read(filename)
    last = src
    last = core.std.ShufflePlanes(last, planes=0, colorfamily=vs.GRAY)
    if [last.width, last.height]!=target:
        if last.height*target[0]==last.width*target[1]:
            width, height = target
            border = [0]*4
        elif last.height*target[0]>last.width*target[1]:
            width = round(last.width/last.height*target[1])
            height = target[1]
            if width%2==1:
                width += 1
            border = [(target[0]-width)/2]*2+[0]*2
        else:
            width = target[0]
            height = round(last.height/last.width*target[0])
            if height%2==1:
                height += 1
            border = [0]*2+[(target[1]-height)/2]*2
        last = core.resize.Bicubic(last, width, height)
        last = core.std.AddBorders(last, border[0],border[1], border[2], border[3], color=[255])
    
    #last = last.fmtc.bitdepth(bits=8,dmode=8,patsize=64)
    last = mvs.Depth(last,depth=4,dither=8)
    return last

clip = convert(inDir+"\\0000.jpg")

for file in [i for i in os.listdir(inDir) if i[-4:]=='.jpg'][1:]:
#for i in range(1, 237):
#    file = str(i)+'_0.png'
    clip += convert(inDir+'\\'+file)
    

wr = core.imwri.Write(clip, imgformat='PNG', filename=outDir+'\\'+'%04d.png', firstnum=0)
wr.set_output()
