def projectName = "triqs"
def dockerName = projectName.toLowerCase();
/* which platform to build documentation on */
def documentationPlatform = "ubuntu-clang"
/* whether to keep and publish the results */
def keepInstall = !env.BRANCH_NAME.startsWith("PR-")

properties([
  disableConcurrentBuilds(),
  buildDiscarder(logRotator(numToKeepStr: '10', daysToKeepStr: '30')),
  pipelineTriggers(keepInstall ? [
    upstream(
      threshold: 'SUCCESS',
      upstreamProjects: '/TRIQS/cpp2py/master,/TRIQS/itertools/unstable,/TRIQS/mpi/unstable,/TRIQS/h5/unstable'
    )
  ] : [])
])

/* map of all builds to run, populated below */
def platforms = [:]

/****************** linux builds (in docker) */
/* Each platform must have a cooresponding Dockerfile.PLATFORM in triqs/packaging */
def dockerPlatforms = ["ubuntu-clang", "ubuntu-gcc"]
/* .each is currently broken in jenkins */
for (int i = 0; i < dockerPlatforms.size(); i++) {
  def platform = dockerPlatforms[i]
  platforms[platform] = { -> node('docker') {
    stage(platform) { timeout(time: 1, unit: 'HOURS') { ansiColor('xterm') {
      checkout scm
      /* construct a Dockerfile for this base */
      sh """
        ( cat packaging/Dockerfile.${env.STAGE_NAME} ; sed '0,/^FROM /d' Dockerfile.build ) > Dockerfile
      """
      /* build and tag */
      def img = docker.build("flatironinstitute/${dockerName}:${env.BRANCH_NAME}-${env.STAGE_NAME}", "--build-arg APPNAME=${projectName} --build-arg BUILD_DOC=${platform==documentationPlatform} --build-arg BUILD_ID=${env.BUILD_TAG} .")
      catchError(buildResult: 'UNSTABLE', stageResult: 'UNSTABLE') {
        img.inside() {
          sh "make -C \$BUILD/${projectName} test CTEST_OUTPUT_ON_FAILURE=1"
        }
      }
      if (!keepInstall) {
        sh "docker rmi --no-prune ${img.imageName()}"
      }
    } } }
  } }
}

/****************** osx builds (on host) */
def osxPlatforms = [
  ["gcc", ['CC=gcc-10', 'CXX=g++-10', 'FC=gfortran-10']],
  ["clang", ['CC=$BREW/opt/llvm/bin/clang', 'CXX=$BREW/opt/llvm/bin/clang++', 'FC=gfortran-10', 'CXXFLAGS=-I$BREW/opt/llvm/include', 'LDFLAGS=-L$BREW/opt/llvm/lib']]
]
for (int i = 0; i < osxPlatforms.size(); i++) {
  def platformEnv = osxPlatforms[i]
  def platform = platformEnv[0]
  platforms["osx-$platform"] = { -> node('osx && triqs') {
    stage("osx-$platform") { timeout(time: 1, unit: 'HOURS') { ansiColor('xterm') {
      def srcDir = pwd()
      def tmpDir = pwd(tmp:true)
      def buildDir = "$tmpDir/build"
      /* install real branches in a fixed predictable place so apps can find them */
      def installDir = keepInstall ? "${env.HOME}/install/${projectName}/${env.BRANCH_NAME}/${platform}" : "$tmpDir/install"
      def venv = installDir

      checkout scm

      def hdf5 = "${env.BREW}/opt/hdf5@1.10"
      dir(buildDir) { withEnv(platformEnv[1].collect { it.replace('\$BREW', env.BREW) } + [
          "PATH=$venv/bin:${env.BREW}/bin:/usr/bin:/bin:/usr/sbin",
          "HDF5_ROOT=$hdf5",
          "C_INCLUDE_PATH=$hdf5/include:${env.BREW}/include",
          "CPLUS_INCLUDE_PATH=$venv/include:$hdf5/include:${env.BREW}/include",
          "LIBRARY_PATH=$venv/lib:$hdf5/lib:${env.BREW}/lib",
          "LD_LIBRARY_PATH=$hdf5/lib",
          "PYTHONPATH=$installDir/lib/python3.9/site-packages",
          "CMAKE_PREFIX_PATH=$venv/lib/cmake/triqs"]) {
        deleteDir()
        sh "python3 -m venv $venv"
        sh "DYLD_LIBRARY_PATH=\$BREW/lib pip3 install -U -r $srcDir/requirements.txt"
        sh "cmake $srcDir -DCMAKE_INSTALL_PREFIX=$installDir -DBuild_Deps=Always"
        sh "make -j2"
        catchError(buildResult: 'UNSTABLE', stageResult: 'UNSTABLE') { try {
          sh "make test CTEST_OUTPUT_ON_FAILURE=1"
        } catch (exc) {
          archiveArtifacts(artifacts: 'Testing/Temporary/LastTest.log')
          throw exc
        } }
        sh "make install"
      } }
    } } }
  } }
}

/****************** sanitization builds (in docker) */
platforms['sanitize'] = { -> node('docker') {
  stage('sanitize') { timeout(time: 1, unit: 'HOURS') { ansiColor('xterm') {
    checkout scm
    /* construct a Dockerfile for this base */
    def img = docker.build("flatironinstitute/${projectName}:${env.BRANCH_NAME}-sanitize", "-f packaging/Dockerfile.sanitize .")
    sh "docker rmi --no-prune ${img.imageName()}"
  } } }
} }

/****************** wrap-up */
def error = null
try {
  parallel platforms
  if (keepInstall) { node("docker") {
    /* Publish results */
    stage("publish") { timeout(time: 5, unit: 'MINUTES') {
      def commit = sh(returnStdout: true, script: "git rev-parse HEAD").trim()
      def release = env.BRANCH_NAME == "master" || env.BRANCH_NAME == "unstable" || sh(returnStdout: true, script: "git describe --exact-match HEAD || true").trim()
      def workDir = pwd(tmp:true)
      lock('triqs_publish') {
      /* Update documention on gh-pages branch */
      dir("$workDir/gh-pages") {
	def subdir = "${projectName}/${env.BRANCH_NAME}"
	git(url: "ssh://git@github.com/TRIQS/TRIQS.github.io.git", branch: "master", credentialsId: "ssh", changelog: false)
	sh "rm -rf ${subdir}"
	docker.image("flatironinstitute/${dockerName}:${env.BRANCH_NAME}-${documentationPlatform}").inside() {
          sh """#!/bin/bash -ex
            base=\$INSTALL/share/doc
            dir="${projectName}"
            [[ -d \$base/triqs_\$dir ]] && dir=triqs_\$dir || [[ -d \$base/\$dir ]]
            cp -rp \$base/\$dir ${subdir}
          """
	}
	sh "git add -A ${subdir}"
	sh """
	  git commit --author='Flatiron Jenkins <jenkins@flatironinstitute.org>' --allow-empty -m 'Generated documentation for ${subdir}' -m '${env.BUILD_TAG} ${commit}'
	"""
	// note: credentials used above don't work (need JENKINS-28335)
        sh "git push origin master"
      }
      /* Update packaging repo submodule */
      if (release) { dir("$workDir/packaging") { try {
	git(url: "ssh://git@github.com/TRIQS/packaging.git", branch: env.BRANCH_NAME, credentialsId: "ssh", changelog: false)
	// note: credentials used above don't work (need JENKINS-28335)
        sh """#!/bin/bash -ex
          dir="${projectName}"
          [[ -d triqs_\$dir ]] && dir=triqs_\$dir || [[ -d \$dir ]]
          echo "160000 commit ${commit}\t\$dir" | git update-index --index-info
          git commit --author='Flatiron Jenkins <jenkins@flatironinstitute.org>' -m 'Autoupdate ${projectName}' -m '${env.BUILD_TAG}'
          git push origin ${env.BRANCH_NAME}
        """
      } catch (err) {
        /* Ignore, non-critical -- might not exist on this branch */
	echo "Failed to update packaging repo"
      } } }
      }
    } }
  } }
} catch (err) {
  error = err
} finally {
  /* send email on build failure (declarative pipeline's post section would work better) */
  if ((error != null || currentBuild.currentResult != 'SUCCESS') && env.BRANCH_NAME != "jenkins") emailext(
    subject: "\$PROJECT_NAME - Build # \$BUILD_NUMBER - FAILED",
    body: """\$PROJECT_NAME - Build # \$BUILD_NUMBER - FAILED

Check console output at \$BUILD_URL to view full results.

Building \$BRANCH_NAME for \$CAUSE
\$JOB_DESCRIPTION

Changes:
\$CHANGES

End of build log:
\${BUILD_LOG,maxLines=60}
    """,
    to: 'oparcollet@flatironinstitute.org, nwentzell@flatironinstitute.org, dsimon@flatironinstitute.org',
    recipientProviders: [
      [$class: 'DevelopersRecipientProvider'],
    ],
    replyTo: '$DEFAULT_REPLYTO'
  )
  if (error != null) throw error
}
