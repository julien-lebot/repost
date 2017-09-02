'use strict';

import { Socket } from 'net';
import * as protobuf from 'protobufjs';
import { StringDecoder }  from 'string_decoder';

var EventEmitter = require('events').EventEmitter;
const decoder = new StringDecoder('utf8');
const q = require('q');
const pathToProto = 'repost.proto';

export class Client extends (EventEmitter as { new(): any; }) {
    socket: Socket;
    messageTemplate: any;
    publicationTemplate: any;

    decodeMessage(msg: Buffer) {
        //var len = msg.readUInt32LE(0);
        var body = msg.subarray(4);
        var decodedMsg = this.publicationTemplate.decode(body);
        decodedMsg.payload = decoder.write(Buffer.from(decodedMsg.payload, 'base64'));
        return decodedMsg;
    }

    encodeMessage(msg) {
        var msg = this.messageTemplate.create(msg);
        var buf = this.messageTemplate.encode(msg).finish();
        var encodedBuffer = Buffer.alloc(4 + buf.length);
        encodedBuffer.writeUInt32LE(buf.length, 0);
        buf.copy(encodedBuffer, 4, 0, buf.length);
        return encodedBuffer;
    }

    constructor() {
        super();

        var self = this;
        this.socket = new Socket();
        this.socket.on('data', function(data) {
            var msg = self.decodeMessage(data);
            self.emit('data', msg);
        });
    }

    subscribe(channels: string[]) {
        this.socket.write(this.encodeMessage({
            subscribe: {
                Channels: channels
            }
        }));
    }

    unsubscribe(channels: string[]) {
        this.socket.write(this.encodeMessage({
            unsubscribe: {
                Channels: channels
            }
        }));
    }

    publish(channel: string, payload: Buffer) {
        this.socket.write(this.encodeMessage({
            publication: {
                channel: channel,
                payload: payload
            }
        }));
    }

    connect(host: string, port: number) {
        var deferred = q.defer();
        var self = this;
        protobuf.load(pathToProto)
            .then(function(root) {
                self.messageTemplate = root.lookupType('repost.Message');
                self.publicationTemplate = root.lookupType('repost.Publication');
                self.socket.connect(port, host, function() {
                    deferred.resolve();
                });
            });

        this.socket.once('error', function() {
            deferred.reject();
        });

        return deferred.promise;
    }
};
