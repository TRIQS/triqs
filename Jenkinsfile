pipeline {
  agent none
  options {
    disableConcurrentBuilds()
    timeout(time: 1, unit: 'HOURS')
  }
  stages {
    stage('build') {
      parallel {
        stage('ubuntu') {
          agent {
            dockerfile {
              dir "packaging/ubuntu"
            }
          }
          steps {
            sh '''#!/bin/bash -ex
              source $INSTALL/share/cpp2pyvars.sh
              mkdir -p build
              cd build
              cmake .. -DCMAKE_INSTALL_PREFIX=$INSTALL
              make
              make test
            '''
          }
        }
      }
    }
  }
}
