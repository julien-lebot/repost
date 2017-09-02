'use strict';

import { Client as RepostClient } from './index';
const argv = require('yargs').argv
var readline = require('readline');

function printUsage() {
    console.log('usage: node repost-cli.js --server=<server:port>');
    console.log('example: node repost-cli.js --server=127.0.0.1:4242');
}

if (!argv.server) {
    printUsage();
    process.exit(1);
}

var server = argv.server.split(':');
if (server.length != 2) {
    printUsage();
    process.exit(1);
}

var client = new RepostClient();

client.on('data', function(msg) {
    console.log(msg.channel + ': ' + msg.payload);
});

client.connect(server[0], server[1]).then(function() {
    var rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout,
        terminal: false
      });
      
      rl.on('line', function(line) {
        var tokens = line.trim().split(' ');
        if (tokens.length >= 3) {
            if (tokens[0].startsWith('PUB')) {
                client.publish(tokens[1], new Buffer(line.substring(line.indexOf(tokens[2])), 'ascii'));
            }
        } else if (tokens.length >= 2) {
            if (tokens[0].startsWith('SUB')) {
                client.subscribe(tokens.slice(1));
            } else if (tokens[0].startsWith('UNSUB')) {
                client.unsubscribe(tokens.slice(1));
            }
        }
      })
});