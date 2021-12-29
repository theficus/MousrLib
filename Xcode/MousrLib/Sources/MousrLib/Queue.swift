//
//  Queue.swift
//  
//
//  Created by Adam Meltzer on 12/27/2021.
//

import Foundation

// How is there no Queue<T> in the Swift standard library?
public struct Queue<T> {
    private var items : [T] = []
    private let lock : DispatchSemaphore = DispatchSemaphore(value: 1)

    public init() {
    }

    public mutating func enqueue(_ element: T) {
        lock.with {
            self.items.append(element)
        }
    }

    public func isEmpty() -> Bool {
        return lock.with {
            items.isEmpty
        }
    }

    public func peek() -> T? {
        return lock.with {
            items.isEmpty ? nil : items.first
        }
    }

    public mutating func clear() {
        return lock.with {
            items.removeAll()
        }
    }

    public func size() -> Int {
        return lock.with {
            return items.count
        }
    }

    public mutating func dequeue() -> T? {
        return lock.with {
            items.isEmpty ? nil : items.removeFirst()
        }
    }
}
