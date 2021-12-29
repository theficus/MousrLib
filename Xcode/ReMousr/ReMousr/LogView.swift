//
//  LogView.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/04/2021.
//

import SwiftUI
import MousrLib

struct LogView: View {
    @EnvironmentObject var model : DeviceModel

    var body: some View {
        VStack {
            Menu {
                Toggle(isOn: $model.isLogUnknown, label: {
                    Text("Log Unknown")
                })
                Toggle(isOn: $model.isLogBattery, label: {
                    Text("Log Battery")
                })
                Toggle(isOn: $model.isLogMovement, label: {
                    Text("Log Movement")
                })
                Toggle(isOn: $model.isLogVerbose, label: {
                    Text("Log Verbose")
                })
            } label: {
                Text("Log Settings")
            }

            ScrollViewReader {
                scrollView in VStack {
                    ScrollView(.vertical) {
                        ForEach(model.messages) { message in
                            HStack {
                                Text(String(describing: message.messageDate))
                                Text(String(describing: message.messageType))
                                Text(message.message)
                            }
                            .frame(maxWidth: .infinity, alignment: .leading)
                        }
                    }.id("LogMessageView")
                }.onChange(of: model.messages) { _ in
                    scrollView.scrollTo("LogMessageView", anchor: .bottom)
                }
            }
            .font(.system(size: 10))
            .frame(maxWidth: .infinity, alignment: .leading)
            .background(Color(UIColor.systemGray6))

            Button("Clear") {
                model.clearLog()
            }
        }
    }
}

struct LogView_Previews: PreviewProvider {
    static var model = DeviceModel()
    static var previews: some View {
        LogView()
            .environmentObject(model)
    }
}
