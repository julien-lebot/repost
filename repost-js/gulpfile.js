var gulp = require('gulp');
var ts = require('gulp-typescript');
var tsProject = ts.createProject('tsconfig.json');

gulp.task('copy:support_files', function() {
    return gulp.src(['../repost-common/repost.proto', 'package.json'])
        .pipe(gulp.dest('dist'));
})

gulp.task('tsc:compile', function() {
    return tsProject.src()
    .pipe(tsProject())
    .js.pipe(gulp.dest("dist"));
})

gulp.task('default', ['copy:support_files', 'tsc:compile']);