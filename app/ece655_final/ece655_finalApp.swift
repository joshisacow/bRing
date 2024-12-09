//
//  ece655_finalApp.swift
//  ece655_final
//
//  Created by Emily Shao on 11/8/24.
//

import SwiftUI

@main
struct ece655_finalApp: App {
    @State private var isLoggedIn = false
    
    var body: some Scene {
        WindowGroup {
            if isLoggedIn {
                ContentView()
            } else {
                LoginView()
                    .onAppear {
                        DispatchQueue.main.asyncAfter(deadline: .now() + 2) {
                            // login logic
                            isLoggedIn = true
                        }
                    }
            }
        }
    }
}


