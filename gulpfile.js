var gulp = require('gulp');
var proc = require('child_process');
const fs = require('fs');

var isWin = /^win/.test(process.platform);

gulp.task('generate_code', function()
{
    if (isWin)
    {
        proc.exec('.\\protobuf\\cmake\\build\\Release\\protoc.exe --cpp_out="repost-common\\generated\\cpp" --csharp_out="repost-common\\generated\\cs" --js_out="repost-common\\generated\\js" repost-common\\repost.proto',
        function(error, stdout, stderr)
        {
            if (error)
            {
                console.log(error);
            }
        });
    }
    else
    {
        proc.exec('protoc --cpp_out="repost-common/generated/cpp" --csharp_out="repost-common/generated/cs" --js_out="repost-common/generated/js" repost-common/repost.proto',
        function(error, stdout, stderr)
        {
            if (error)
            {
                console.log(error);
            }
        });
    }
})