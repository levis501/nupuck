#!/usr/bin/env python

"""Dummy socket server.  repeats input to output"""

import socket
import os
from nupic.frameworks.opf.modelfactory import ModelFactory

SOCKET_NAME="/tmp/nupuck.socket"
LPOS20_PARAMS = {'aggregationInfo': {'days': 0,
                     'fields': [],
                     'hours': 0,
                     'microseconds': 0,
                     'milliseconds': 0,
                     'minutes': 0,
                     'months': 0,
                     'seconds': 0,
                     'weeks': 0,
                     'years': 0},
 'model': 'CLA',
 'modelParams': {'anomalyParams': {u'anomalyCacheRecords': None,
                                   u'autoDetectThreshold': None,
                                   u'autoDetectWaitRecords': None},
                 'clParams': {'alpha': 0.008887987798496312,
                              'clVerbosity': 0,
                              'regionName': 'CLAClassifierRegion',
                              'steps': '20'},
                 'inferenceType': 'TemporalMultiStep',
                 'sensorParams': {'encoders': {'_classifierInput': {'classifierOnly': True,
                                                                    'clipInput': True,
                                                                    'fieldname': 'lpos',
                                                                    'n': 77,
                                                                    'name': '_classifierInput',
                                                                    'type': 'AdaptiveScalarEncoder',
                                                                    'w': 21},
                                               u'lpos': {'clipInput': True,
                                                         'fieldname': 'lpos',
                                                         'n': 521,
                                                         'name': 'lpos',
                                                         'type': 'AdaptiveScalarEncoder',
                                                         'w': 21},
                                               u'lwidth': {'clipInput': True,
                                                           'fieldname': 'lwidth',
                                                           'n': 71,
                                                           'name': 'lwidth',
                                                           'type': 'AdaptiveScalarEncoder',
                                                           'w': 21},
                                               u'rpos': None,
                                               u'rwidth': None},
                                  'sensorAutoReset': None,
                                  'verbosity': 0},
                 'spEnable': True,
                 'spParams': {'columnCount': 2048,
                              'globalInhibition': 1,
                              'inputWidth': 0,
                              'maxBoost': 2.0,
                              'numActiveColumnsPerInhArea': 40,
                              'potentialPct': 0.8,
                              'seed': 1956,
                              'spVerbosity': 0,
                              'spatialImp': 'cpp',
                              'synPermActiveInc': 0.05,
                              'synPermConnected': 0.1,
                              'synPermInactiveDec': 0.1},
                 'tpEnable': True,
                 'tpParams': {'activationThreshold': 12,
                              'cellsPerColumn': 32,
                              'columnCount': 2048,
                              'globalDecay': 0.0,
                              'initialPerm': 0.21,
                              'inputWidth': 2048,
                              'maxAge': 0,
                              'maxSegmentsPerCell': 128,
                              'maxSynapsesPerSegment': 32,
                              'minThreshold': 9,
                              'newSynapseCount': 20,
                              'outputType': 'normal',
                              'pamLength': 2,
                              'permanenceDec': 0.1,
                              'permanenceInc': 0.1,
                              'seed': 1960,
                              'temporalImp': 'cpp',
                              'verbosity': 0},
                 'trainSPNetOnlyIfRequested': False},
 'predictAheadTime': None,
 'version': 1}

RPOS20_PARAMS = {'aggregationInfo': {'days': 0,
                     'fields': [],
                     'hours': 0,
                     'microseconds': 0,
                     'milliseconds': 0,
                     'minutes': 0,
                     'months': 0,
                     'seconds': 0,
                     'weeks': 0,
                     'years': 0},
 'model': 'CLA',
 'modelParams': {'anomalyParams': {u'anomalyCacheRecords': None,
                                   u'autoDetectThreshold': None,
                                   u'autoDetectWaitRecords': None},
                 'clParams': {'alpha': 0.01183971581823866,
                              'clVerbosity': 0,
                              'regionName': 'CLAClassifierRegion',
                              'steps': '20'},
                 'inferenceType': 'NontemporalMultiStep',
                 'sensorParams': {'encoders': {'_classifierInput': {'classifierOnly': True,
                                                                    'clipInput': True,
                                                                    'fieldname': 'rpos',
                                                                    'n': 64,
                                                                    'name': '_classifierInput',
                                                                    'type': 'AdaptiveScalarEncoder',
                                                                    'w': 21},
                                               u'lpos': {'clipInput': True,
                                                         'fieldname': 'lpos',
                                                         'n': 517,
                                                         'name': 'lpos',
                                                         'type': 'AdaptiveScalarEncoder',
                                                         'w': 21},
                                               u'lwidth': {'clipInput': True,
                                                           'fieldname': 'lwidth',
                                                           'n': 112,
                                                           'name': 'lwidth',
                                                           'type': 'AdaptiveScalarEncoder',
                                                           'w': 21},
                                               u'rpos': None,
                                               u'rwidth': {'clipInput': True,
                                                           'fieldname': 'rwidth',
                                                           'n': 117,
                                                           'name': 'rwidth',
                                                           'type': 'AdaptiveScalarEncoder',
                                                           'w': 21}},
                                  'sensorAutoReset': None,
                                  'verbosity': 0},
                 'spEnable': True,
                 'spParams': {'columnCount': 2048,
                              'globalInhibition': 1,
                              'inputWidth': 0,
                              'maxBoost': 2.0,
                              'numActiveColumnsPerInhArea': 40,
                              'potentialPct': 0.8,
                              'seed': 1956,
                              'spVerbosity': 0,
                              'spatialImp': 'cpp',
                              'synPermActiveInc': 0.05,
                              'synPermConnected': 0.1,
                              'synPermInactiveDec': 0.09516831738960979},
                 'tpEnable': True,
                 'tpParams': {'activationThreshold': 13,
                              'cellsPerColumn': 32,
                              'columnCount': 2048,
                              'globalDecay': 0.0,
                              'initialPerm': 0.21,
                              'inputWidth': 2048,
                              'maxAge': 0,
                              'maxSegmentsPerCell': 128,
                              'maxSynapsesPerSegment': 32,
                              'minThreshold': 9,
                              'newSynapseCount': 20,
                              'outputType': 'normal',
                              'pamLength': 1,
                              'permanenceDec': 0.1,
                              'permanenceInc': 0.1,
                              'seed': 1960,
                              'temporalImp': 'cpp',
                              'verbosity': 0},
                 'trainSPNetOnlyIfRequested': False},
 'predictAheadTime': None,
 'version': 1}

def loadModels():
  global lpos20_model, rpos20_model
  lpos20_model = ModelFactory.create(LPOS20_PARAMS)
  rpos20_model = ModelFactory.create(RPOS20_PARAMS)
  lpos20_model.enableInference({'predictedField': 'lpos'})
  rpos20_model.enableInference({'predictedField': 'rpos'})

def process(data):
  global lpos20_model, rpos20_model
  (lpos,lwidth,rpos,rwidth) = [float(field) for field in data.split()]
  model_input = {'lpos':lpos,'lwidth':lwidth,'rpos':rpos,'rwidth':rwidth}
  lresult = lpos20_model.run(model_input)
  rresult = rpos20_model.run(model_input)
  lpos_prediction = lresult.inferences['multiStepBestPredictions'][20]
  rpos_prediction = rresult.inferences['multiStepBestPredictions'][20]
  return "%f %f %f %f\n" % (lpos_prediction,lwidth,rpos_prediction,rwidth)

if __name__=='__main__':
  loadModels()
  sock = socket.socket( socket.AF_UNIX, socket.SOCK_STREAM)
  try:
    os.remove(SOCKET_NAME)
  except OSError:
    pass
  sock.bind(SOCKET_NAME)
  sock.listen(1)
  while True:
    conn, addr = sock.accept()
    while 1:
      data = conn.recv(1024)
      if not data:
        break
      conn.send(process(data))

