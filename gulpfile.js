var gulp = require('gulp');

var isWindows = /^win/.test(process.platform);
if (isWindows) {
    require('./windows-gulpfile.js');
}

gulp.task('default', ['build']);