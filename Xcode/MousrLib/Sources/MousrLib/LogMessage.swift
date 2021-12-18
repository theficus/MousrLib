//
//  LogMessage.swift
//
//  Created by Adam Meltzer on 12/08/2021.
//

import Foundation

// MARK: Log message type
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

// Simple struct to represent a unique log message
public struct LogMessage : Hashable, Identifiable {
    private static var lastId : Int64 = 0 // Simple message counter

    // Unique message ID. Not guaranteed to be sorted chronologically -- use messageDate instead if you need true chronological sorting
    public let id : Int64

    // Message creation date
    public let messageDate : Date

    // Message type
    public let messageType : LogMessageType

    // Message text
    public let message : String

    public init(message : String,
         messageType : LogMessageType) {
        self.id = OSAtomicIncrement64(&LogMessage.lastId)
        self.messageDate = Date.now
        self.message = message
        self.messageType = messageType
    }
}
