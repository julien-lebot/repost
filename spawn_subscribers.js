var proc = require('child_process');
var argv = require('minimist')(process.argv.slice(2));

if (!argv.count)
{
    console.log('usage: spawn_subscribers.js [count]')
    return;
}

function spawn(index)
{
    var child = proc.spawn('Debug\\repost-cli.exe', ['--host=127.0.0.1', '--port=4242', '--channel=test']);
    child.stdout.on('data', function(data)
    {
        console.log('stdout[' + index + ']:' + data);
    });
    child.stderr.on('data', function(data)
    {
        console.log('stderr[' + index + ']:' + data);
    });
    child.on('close', function(code)
    {
        console.log('closing process [' + index + ']:' + code);
    });
}

for (var i = 0; i < argv.count; ++i)
{
    spawn(i);
};