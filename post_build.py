
print ("Adding Post Build: Applying the CRC to the firmware")


from shutil import copyfile
from datetime import date


import sys
import binascii
import struct
from hashlib import md5
import os
MD5DUMMY      = b"MD5_MD5_MD5_MD5_BoundariesOfTheSegmentsGoHere..." #48 chars
FILENAMEDUMMY = b"ThisIsTheDummyPlaceHolderForTheBinaryFilename64ByteLongFilenames" #64 chars

MemorySegmentStart,MemorySegmentEnd,MemoryContent=[],[],[]


print("Python version:",sys.version_info.major,".",sys.version_info.minor)

##################################################################
# this subroutine shows the segments of a  part
##################################################################
def showSegments (fileContent,offset):
    global MemorySegmentStart, MemorySegmentEnd, MemoryContent
    header = struct.unpack("ii", fileContent[offset:offset+8])
    herestr =""
    herestr2 =""
    MemorySegmentStart.append(struct.pack("I",header[0]))
    MemorySegmentEnd.append(struct.pack("I",header[0]+header[1]))
    MemoryContent.append(fileContent[offset+8:offset+8+header[1]])
    if  fileContent.find( MD5DUMMY, offset+8, offset+8+header[1]) >0 :
        herestr= " <-- CRC is here."
    if  fileContent.find( FILENAMEDUMMY, offset+8, offset+8+header[1]) >0 :
        herestr2= " <-- filename is here."
    print ("SEGMENT "+ str(len(MemorySegmentStart)-1)+ ": memory position: " + hex(header[0])+" to " + hex(header[0]+header[1]) +  " length: " + hex(header[1])+herestr+herestr2)
    #print ("first byte positoin in file: " + hex( offset+8))
    #print ("last byte postion in file: " + hex(offset+8+header[1]-1))
    return (8+offset+ header[1]); # return start of next segment

##################################################################
# this subroutine shows the  parts of a binary file
##################################################################
def showParts(fileContent, offset):
    
    
    print ('\n\nBINARY PART\nSegments: ') , (hex(fileContent[offset+1]))
    nextpos =offset+8
    for x in range (0,fileContent[offset+1]):
        nextpos = showSegments(fileContent,nextpos)
    nextSegmentOffset = (fileContent.find(b"\xe9", nextpos))
    return nextSegmentOffset


def applyCRC2(FileName):
    with open(FileName, mode='rb') as file: # b is important -> binary
        nextpos =0
        fileContent = file.read()
        while nextpos >=0:
            nextpos = showParts(fileContent,nextpos)
    startArray,endArray,hashString = b"",b"",b""
    includeStr = "hash includes segments:"
    # memory sections:
    # 0: bootloader (not readable)
    # 1: program memory (SPI flash)
    # 2: unknown but stable
    # 3: RAM (initialized by bin file. Can be read but changes as you go :-( )
    # 4: RAM

    for i in (1,2 ):     # use only stable segments, must be 4 in total. We use 2.
        startArray =startArray + MemorySegmentStart[i]
        endArray =  endArray   + MemorySegmentEnd[i]
        hashString =hashString + MemoryContent[i]
        with open(FileName+str(i), mode='wb') as file: # b is important -> binary
            file.write(MemoryContent[i])
        includeStr = includeStr +" "+ str(i)
    print (includeStr)
    # IMPORTANT: pad array with zeros if you use only 3 segments (see above)
    while len(startArray) < 16 :
        startArray =startArray + struct.pack("I",0)
        endArray =  endArray   + struct.pack("I",0)
    # debug print (binascii.hexlify(startArray))
    # debug print (binascii.hexlify(endArray))
    if (len(endArray) + len (startArray)) != 32 :
        print("ERROR: please make sure you add / remove padding if you change the segments.")

    BinaryFileName = "";
    #if  fileContent.find( FILENAMEDUMMY.encode(encoding='utf-8',errors='strict')) < 0:
    if  fileContent.find( FILENAMEDUMMY) < 0:
        print("ERROR: FILENAMEDUMMY dummy not found in binary")
    else:
        BinaryFileName=os.path.basename(FileName) +"\0"
        if len(BinaryFileName) >64:								# check that filename is <48 chars
            BinaryFileName=BinaryFileName[0:64]					# truncate if necessary. 49th char in ESP is zero already
        else:
            BinaryFileName= BinaryFileName.ljust(64,'\0');		# pad with zeros.


    #if  fileContent.find( MD5DUMMY.encode(encoding='utf-8',errors='strict')) < 0:
    if  fileContent.find( MD5DUMMY) < 0:
        print("ERROR: MD5 dummy not found in binary")
    else:
        #hashString=hashString.replace (MD5DUMMY.encode(encoding='utf-8',errors='strict'),b"",1)
        hashString=hashString.replace (MD5DUMMY,b"",1)
        m = md5(hashString)
        #m.update (hashString) #use segment 1
        md5hash = m.digest()
        print("MD5 hash: "+ m.hexdigest())
        print("\nwriting output file:\n" + FileName)

        #fileContent=fileContent.replace(MD5DUMMY.encode(encoding='utf-8',errors='strict'),md5hash+startArray+endArray)
        #fileContent=fileContent.replace(FILENAMEDUMMY.encode(encoding='utf-8',errors='strict'),BinaryFileName.encode(encoding='utf-8',errors='strict'))

        fileContent=fileContent.replace(MD5DUMMY,md5hash+startArray+endArray)
        fileContent=fileContent.replace(FILENAMEDUMMY,BinaryFileName.encode(encoding='utf-8',errors='strict'))


        with open(FileName, mode='wb') as file: # b is important -> binary
            file.write(fileContent)
    #k=input("press close to exit")
    

def crc_firmware(source, target, env):
    #print("Source: "+source)
    #print(env.Dump())
    print(".pio/build/"+env['PIOENV']+"/firmware.bin")
    bin = env['PROJECT_DIR']+"/dist/"+env['PIOENV']+date.today().strftime("_%Y_%m_%d") +".bin"
    copyfile(".pio/build/"+env['PIOENV']+"/firmware.bin", bin)
    #os.system("crc2.py " + bin)
    
    print("MFD: TODO: fix the crc2 python script to support v3.8 !!!!!!")
    applyCRC2(bin)
    os.remove(bin+'1')
    os.remove(bin+"2")
    
    print ("Done!")



Import("env", "projenv")

# access to global build environment
#print(env)

# access to project build environment (is used source files in "src" folder)
#print(projenv)

#
# Dump build environment (for debug purpose)
# print(env.Dump())
#


# Apply crc MD5 to the firmware file
env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin",crc_firmware)


