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
              args '-v /etc/passwd:/etc/passwd -v /etc/group:/etc/group'
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
        stage('centos') {
          agent {
            dockerfile {
              dir "packaging/centos"
              args '-v /etc/passwd:/etc/passwd -v /etc/group:/etc/group'
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
