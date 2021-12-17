//
//  LogMessage.swift
//  IOSSwiftUIPlayground
//
//  Created by Adam Meltzer on 12/08/2021.
//

import Foundation

public enum LogMessageType {
    case unknown
    case debug
    case verbose
    case move
    case info
    case battery
    case error
    case warning
}

public struct LogMessage : Hashable, Identifiable {
    private static var lastId : Int64 = 0
    public let id : Int64
    var messageDate : Date
    var messageType : LogMessageType
    var message : String

    init(message : String,
         messageType : LogMessageType) {
        self.id = OSAtomicIncrement64(&LogMessage.lastId)
        self.messageDate = Date.now
        self.message = message
        self.messageType = messageType
    }
}
