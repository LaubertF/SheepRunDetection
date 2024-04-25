import os


def getRegularData():
    PATH = "Data"
    all_files = [os.path.join(dp, f) for dp, dn, filenames in os.walk(PATH) for f in filenames if
                 os.path.splitext(f)[1] == '.txt' and "_Regular" in f]
    return [all_files[i] for i in range(0, len(all_files) - 1, 2)]


def getIrregularData():
    PATH = "Data"
    all_files = [os.path.join(dp, f) for dp, dn, filenames in os.walk(PATH) for f in filenames if
                 os.path.splitext(f)[1] == '.txt' and "_Irregular" in f]
    return [all_files[i] for i in range(0, len(all_files) - 1, 2)]


def getSemiRegularData():
    PATH = "Data"
    all_files = [os.path.join(dp, f) for dp, dn, filenames in os.walk(PATH) for f in filenames if
                 os.path.splitext(f)[1] == '.txt' and "_SemiRegular" in f]
    return [all_files[i] for i in range(0, len(all_files) - 1, 2)]


def getOxData():
    PATH = "DataOx\\Hip_100Hz"
    all_files = os.listdir(PATH)
    return [PATH + "\\" + file for file in all_files]

def getSheepData():
    PATH = "Sheep"
    all_files = [os.path.join(dp, f) for dp, dn, filenames in os.walk(PATH) for f in filenames if
                 "output" in f]
    return all_files


def getFilterFor76hz():
    return [0.0158467315853596, 0.0457943754888393, 0.121235976281198, 0.200244477464560, 0.233756878360087,
            0.200244477464560, 0.121235976281198, 0.0457943754888393, 0.0158467315853596]


def getFilterFor100hz():
    return [0.0167855014066363, 0.0470217363830679, 0.121859539593060, 0.198782647391950, 0.231101150450572,
            0.198782647391950, 0.121859539593060, 0.0470217363830679, 0.0167855014066363]


def getFilterFor15hz():
    return [-0.00614041469794508, -0.0135816744769618, 0.0512322973427179, 0.265655560905451, 0.405668461853476,
            0.265655560905451, 0.0512322973427179, -0.0135816744769618, -0.00614041469794508]
