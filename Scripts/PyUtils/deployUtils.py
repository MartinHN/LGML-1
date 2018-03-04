import os, glob, json

import gitUtils
from shutil import copy2 , copyfileobj
import zipfile
import gzip
import tempfile 

raiseErrorOnDifferentSha = False;
distPath = os.path.expanduser("~/owncloud/DEVSPECTACLES/Tools/LGML/App-Dev/dist/")
desiredVersion = "1.2.7"
lastVPath = os.path.join(distPath,"bleedingEdge",desiredVersion)
# publicFolder = '/Volumes/sshfs/owncloud/tools/LGML/'
publicFolder = '/tmp/LGML/dist'
changeLogPath  = os.path.join(lastVPath,"CHANGELOG.md")

readmePath = os.path.dirname(os.path.abspath(__file__));
readmePath = os.path.join(readmePath,"../../README.md");


allCfgs={}
for c in glob.glob(lastVPath+"/*.cfg"):
  with open(c,'r') as fp:
    allCfgs [os.path.basename(c)]=json.load(fp)

currentSha = gitUtils.getGitSha()

def checkIntegrity(errorOnWrongSha=True):
  global currentSha
  sha = currentSha
  if( not os.path.exists(readmePath)):
    raise NameError("can't find README")
  if( not os.path.exists(changeLogPath)):
    raise NameError("can't find CHANGELOG")

  shas = { k:v["git_sha"] for k,v in allCfgs.items()}
  print (shas)
  for k,v in shas.items():
    if v!= currentSha:
      print ("warning sha is not current : %s vs %s"%(v,currentSha))
      break;

  
  for k,v in shas.items():
    currentSha = sha
    if sha!= v:
      err = "mismatching sha for %s: %s vs %s"%(k,sha,v)
      if errorOnWrongSha:
        raise NameError(err)
      else:
        print (err)


  bins = {k:os.path.join(lastVPath,os.path.basename(allCfgs[k]["packaged_name"])) for k in allCfgs.keys()}
  for k,b in bins.items():
    if not os.path.exists(b):
      raise NameError("not found bin for cfg : %s"%b)
    allCfgs[k]["local_bin"] = b;

  zips = {k:os.path.join(lastVPath,k[:-4]+".zip") for k in allCfgs.keys()}
  for k,z in zips.items():
    if not os.path.exists(z):
      raise NameError("not found zip for cfg : %s"%z)
    allCfgs[k]["local_zip"] = z;

  exported_zips = {}
  for k,v in allCfgs.items():
    print(k)
    bid= v["build_version_uid"]
    cf = v["build_cfg_name"]
    if "Release" in cf:
      if bid in exported_zips.keys():
        raise NameError("duplicate build")
      exported_zips[bid] = cf

  return bins,zips

bins,zips = checkIntegrity(raiseErrorOnDifferentSha)
with open(changeLogPath,'r') as fp:
  notes = ''.join(fp.readlines())

def createJSON(destFolder,bins,zips):
  destFolder = destFolder
  v = {"git_sha":currentSha,
      "notes":notes,
      "version":desiredVersion,
      "download_page" : "http://organic-orchestra.com/forum/d/6-lgml-telechargements",
      "zip_link" : {k:os.path.basename(zips[k]) for k in zips.keys()}
      }
  vf = os.path.join(destFolder,"version.json")
  with open(vf,'w') as fp:
    json.dump(v,fp,indent=4)
  return vf


def printReleaseMessage():
  baseLink = "https://storage.organic-orchestra.com/tools/LGML/"+desiredVersion+"/"
  msg = "New LGML is Out : v"+str(desiredVersion)+"\n"
  msg+=notes
  msg+='\n\n'
  builds = {}
  for c,v in allCfgs.items():
    typ = v["build_os"]
    if 'Rpi' in c:
      typ = "Raspberry"
    if(not typ in builds):
      builds[typ] = []
    builds[typ]+=[c]
  for k,v in builds.items():
    msg+=k+" : \n"
    for bn in v:
      msg+=baseLink+bn[:-4]+'\n'
    msg+='\n'
  print (msg)


def deployBins():

  global allCfgs
  vpublicFolder = os.path.join(publicFolder,desiredVersion)
  if not os.path.exists(vpublicFolder):
    os.makedirs(vpublicFolder)
  jsonF = createJSON(vpublicFolder,bins,zips);
  copy2(changeLogPath,vpublicFolder)

  for k,c in allCfgs.items():
    copy2(c["local_bin"],os.path.join(vpublicFolder,os.path.basename(c["local_bin"])))
    copy2(c["local_zip"],os.path.join(vpublicFolder,c["build_version_uid"]))





if __name__ == '__main__':
  
  
  printReleaseMessage()
  deployBins()
  