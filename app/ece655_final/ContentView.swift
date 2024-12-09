//
//  ContentView.swift
//  ece655_final
//
//  Created by Emily Shao on 11/8/24.
//

import SwiftUI

struct ContentView: View {
    @StateObject private var pollingManager = LongPollingManager()
    
    var body: some View {
        VStack {
            TabView {
                HomeView(pollingManager: pollingManager)
                    .tabItem {
                        Label("Home", systemImage: "house")
                    }
                
                StatsView()
                    .tabItem {
                        Label("Data", systemImage: "person")
                    }
            
                NotificationView(notificationStatus: $pollingManager.notificationStatus)
                    .tabItem {
                        Label("Notifications", systemImage: "bell")
                    }
            }
            .onAppear {
                pollingManager.startLongPolling()
//                pollingManager.stopPolling()
            }
//        }
        
        
    }
}

//struct HomeView_Previews: PreviewProvider {
//    static var previews: some View {
//        ContentView()
    }
//}

#Preview {
    ContentView()
}


class LongPollingManager: ObservableObject {
    @Published var notificationStatus: Bool = false
    @Published var img: UIImage? = UIImage()

    private var isPolling = true
    
    func startLongPolling() {
        guard isPolling else { return }

        guard let url = URL(string: "http://10.197.43.108:8080/guest-verification") else {
            print("Invalid URL")
            return
        }

        let task = URLSession.shared.dataTask(with: url) { data, response, error in
            if let error = error {
                print("Error during long polling: \(error.localizedDescription)")
                DispatchQueue.main.asyncAfter(deadline: .now() + 3) {
                    // Retry after a delay
                    self.startLongPolling()
                }
                return
            }
//            , let responseString = String(data: data, encoding: .utf8)
            if let data = data,
                let image = UIImage(data: data) {
                    print(data)
                    DispatchQueue.main.async {
                        self.notificationStatus = true
                        self.img = image
//                      self.stopPolling()
                }
            }

            // Continue polling after a delay to avoid overwhelming the system
            DispatchQueue.main.asyncAfter(deadline: .now() + 1) {
                self.startLongPolling()
            }
        }
        task.resume()
    }

    func stopPolling() {
        isPolling = false
    }

    func resumePolling() {
        if !isPolling {
            isPolling = true
            startLongPolling()
        }
    }
    
    func clearImage() {
        img = nil
    }
}
