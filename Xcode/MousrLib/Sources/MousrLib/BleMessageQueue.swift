//
//  File.swift
//  
//
//  Created by Adam Meltzer on 12/28/2021.
//

import Foundation
import CoreBluetooth

/*
public protocol BleMessageQueueDelegate {
    func onReceived(queue : BleMessageQueue, queueSize : Int)
}
 */

public final class BleMessageQueue {
    private let dispatchQueue : DispatchQueue
    private let queueSemaphore : DispatchSemaphore
    private var messageQueue : Queue<BleMessage>
    private var isRunning : Bool
    private var isCancelRequested : Bool

    public let signal : DispatchSemaphore

    public init() {
        self.dispatchQueue = DispatchQueue.init(label: "messageQueue")
        self.queueSemaphore = DispatchSemaphore(value: 1)
        self.signal = DispatchSemaphore(value: 1)
        self.isRunning = false
        self.isCancelRequested = false
        self.messageQueue = Queue<BleMessage>()
    }

    public func begin() {
        print("Starting message queue thread ...")
        self.isRunning = true
        self.dispatchQueue.async {
            while self.isCancelRequested == false {
                self.queueSemaphore.wait()
                if self.isCancelRequested == false {
                    self.signal.signal()
                }
            }

            print("Message queue thread shutting down ...")
            self.isRunning = false
            self.isCancelRequested = false
        }
    }

    public func enqueue(_ message : BleMessage) {
        self.messageQueue.enqueue(message)
        self.queueSemaphore.signal()
    }

    public func peek() -> BleMessage? {
        return self.messageQueue.peek()
    }

    public func dequeue() -> BleMessage? {
        return self.messageQueue.dequeue()
    }

    public func isEmpty() -> Bool {
        return self.messageQueue.isEmpty()
    }

    public func end() {
        self.isCancelRequested = true
        self.queueSemaphore.signal()
    }
}
