//
//  NotificationView.swift
//  ece655_final
//
//  Created by Emily Shao on 11/8/24.
//

import SwiftUI
import UserNotifications
import UIKit
import Foundation

struct NotificationView: View {
    @Binding var notificationStatus: Bool
    var downloadedImage: UIImage?
    
    let notificationCenter = UNUserNotificationCenter.current()
    @State private var notifications: [String] = []
    
    var body: some View {
        VStack {
            HStack {
                Text("bRing Notifications")
                    .font(.system(size: 34, weight: .bold))
                    .foregroundColor(.white)
                    .padding(.leading)
                Spacer()
            }
            .frame(maxWidth: .infinity, minHeight: 60, idealHeight: 80)
            .background(Color.blue)
            
            List(notifications, id: \.self) { notification in
                Text(notification)
                    .font(.body)
                    .padding()
            }
            .onAppear {
                fetchPastNotifications()
            }

            Spacer()
            
        }
    }
    
    
    func fetchPastNotifications() {
        guard let url = URL(string: "https://example.com/api/notifications") else { return }
        
        URLSession.shared.dataTask(with: url) { data, response, error in
            if let data = data, error == nil {
                if let fetchedNotifications = try? JSONDecoder().decode([String].self, from: data) {
                    DispatchQueue.main.async {
                        self.notifications = fetchedNotifications
                    }
                }
            }
        }.resume()
    }
    
    
    func sendGetRequest(completion: @escaping (UIImage?) -> Void) {
            // Define the URL
            guard let url = URL(string: "http://10.197.43.108:8080/guest-verification") else {
                print("Invalid URL")
                completion(nil)
                return
            }

            // Create a URLSession data task
            let task = URLSession.shared.dataTask(with: url) { data, response, error in
                // Handle errors
                if let error = error {
                    print("Error: \(error.localizedDescription)")
                    completion(nil)
                    return
                }

                // Ensure there is data
                guard let data = data else {
                    print("No data received")
                    completion(nil)
                    return
                }

                // Convert data to UIImage
                if let image = UIImage(data: data) {
//                    self.downloadedImage = image
                    completion(image) // Pass the image to the completion handler
                } else {
                    print("Failed to convert data to UIImage")
                    completion(nil)
                }
            }

            // Start the task
            task.resume()
        }
}


//
//struct NotificationView_Previews: PreviewProvider {
//    @Binding var isActive: Bool
//    
//    static var previews: some View {
//        NotificationView(isActive)
//    }
//}

