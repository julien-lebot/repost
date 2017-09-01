const gulp = require('gulp');
const fs = require('fs');
const mkdirp = require('mkdirp');
const msbuild = require("gulp-msbuild");
const request = require('request');
const source = require('vinyl-source-stream');
const gunzip = require('gulp-gunzip');
const untar = require('gulp-untar');
const argv = require('yargs').argv;
const spawn = require('child_process').spawn;
const q = require('q');

const protobufRootDir = './deps/protobuf/';
const protobufBuildDir = protobufRootDir + '/cmake/build/';
const protobuf = {
    sourceUrl: 'https://github.com/google/protobuf/archive/v3.4.0.tar.gz',
    solutions: {
        protoc: {
            Solution: "protoc.vcxproj",
            Configurations:
            [
                "Debug",
                "Release"
            ]
        },
        protobuf: {
            Solution: "libprotobuf.vcxproj",
            Configurations:
            [
                "Debug",
                "Release"
            ]
        },
        'protobuf-lite': {
            Solution: "libprotobuf-lite.vcxproj",
            Configurations:
            [
                "Debug",
                "Release"
            ]
        }
    },
    protoc: protobufBuildDir + ((argv.release === undefined) ? 'Debug' : 'Release') + '/protoc.exe'
};

function executeAsync(command, args) {
    var deferred = q.defer();

    var childProcess = spawn(command, args);

    childProcess.on('close', function (code, signal) {
        deferred.resolve(code, signal);
    });

    childProcess.on('error', function (err) {
        deferred.reject(err);
    });

    childProcess.stdout.pipe(process.stdout);
    childProcess.stderr.pipe(process.stderr);

    return deferred.promise;
};

function buildProject(project) {
    return gulp.src(project.Solution)
        .pipe(msbuild({
            configuration: (argv.release === undefined) ? project.Configurations[0] : project.Configurations[1],
            targets: ['Build'],
            toolsVersion: 15.0,
            logCommand: true,
            stdout: true
        }));
};

gulp.task('generate:proto', function()
{
    return executeAsync(protobuf.protoc, [
        '--cpp_out=repost-common/generated/cpp',
        '--csharp_out=repost-common/generated/cs',
        '--js_out=repost-common/generated/js',
        'repost-common/repost.proto'
    ]);
});

gulp.task('get:protobuf', function () {
    var deferred = q.defer();
    
    fs.lstat(protobufRootDir, function (err, stats) {
        if (err || !stats.isDirectory()) {
            request(protobuf.sourceUrl)
                .pipe(source('protobuf.tar.gz'))
                .pipe(gunzip())
                .pipe(untar())
                .pipe(gulp.dest('./deps'))
                .on('end', function () {
                    fs.rename('./deps/protobuf-3.4.0', './deps/protobuf',
                        function () {
                            deferred.resolve();
                        });
                });
        }
        else {
            deferred.resolve();
        }
    });

    return deferred.promise;
});

gulp.task('cmake:protobuf', ['get:protobuf'], function () {
    var deferred = q.defer();

    mkdirp(protobufBuildDir, function (err) {
        if (!err) {
            process.chdir(protobufBuildDir);
            console.log('In ' + process.cwd());
            executeAsync('cmake',
                    ['-G', 'Visual Studio 15 2017', '-DCMAKE_INSTALL_PREFIX=../../../install', '-Dprotobuf_BUILD_TESTS=OFF', '..'])
                .then(function () {
                    deferred.resolve();
                });
        } else {
            deferred.reject();
        }
    });

    return deferred.promise;
});

gulp.task('build:protoc', ['cmake:protobuf'], function () {
    return buildProject(protobuf.solutions.protoc);
});

gulp.task('build:protobuf-lite', ['cmake:protobuf'], function() {
    return buildProject(protobuf.solutions["protobuf-lite"]);
});

gulp.task('build:protobuf', ['cmake:protobuf'], function () {
    return buildProject(protobuf.solutions.protobuf);
});

gulp.task('build', ['build:protobuf-lite', 'build:protoc'], function () {

});

