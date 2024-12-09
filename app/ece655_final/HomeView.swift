//
//  HomeView.swift
//  ece655_final
//
//  Created by Emily Shao on 11/8/24.
//

import SwiftUI

struct HomeView: View {
    @ObservedObject var pollingManager: LongPollingManager
    @State private var requestStatus: String?
    
    
    var body: some View {
        VStack {
            HStack {
                Text("bRing Home")
                    .font(.system(size: 34, weight: .bold))
                    .foregroundColor(.white)
                    .padding(.leading)
                Spacer()
            }
            .frame(maxWidth: .infinity, minHeight: 60, idealHeight: 80)
            .background(Color.blue)
            
            if let image = pollingManager.img {
                Image(uiImage: image)  // Show the image
                    .resizable()
                    .scaledToFit()
                    .frame(width: 300, height: 300)  // Adjust size as needed
                    .padding()
//            } else {
//                Text("Waiting for image...")  // Show a loading text if the image is not available
//                    .font(.system(size: 18))
//                    .foregroundColor(.gray)
//                    .padding()
            }
            
            Spacer()
            
            Button(action: {
                print("Door unlock button tapped")
                pollingManager.clearImage()
                sendPostRequest()
            }) {
                Text("Open Sesame")
                    .font(.system(size: 20, weight: .medium))
                    .foregroundColor(.white)
                    .padding()
                    .frame(maxWidth: .infinity)
                    .background(Color.blue)
                    .cornerRadius(10)
            }
            .padding()
            .padding(.bottom, 30)
            
//            Spacer()
            
            if let status = requestStatus {
                Text(status)
                    .font(.system(size: 18))
                    .foregroundColor(status == "Request successful" ? .green : .red)
                    .padding()
            }
            
        }
    }
    
    func sendPostRequest() {
        guard let url = URL(string: "http://10.197.43.108:8080/unlock") else { return }
        
        var request = URLRequest(url: url)
        request.httpMethod = "POST"
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")
        
        URLSession.shared.dataTask(with: request) { data, response, error in DispatchQueue.main.async {
                if let error = error {
                    self.requestStatus = "Request errored: \(error.localizedDescription)"
                    pollingManager.clearImage()
                } else if let response = response as? HTTPURLResponse, (200...299).contains(response.statusCode) {
                    self.requestStatus = "Request successful"
                    DispatchQueue.main.asyncAfter(deadline: .now() + 3) {
                        requestStatus = ""
                    }
                } else {
                    self.requestStatus = "Request failed"
                    pollingManager.clearImage()
                }
            }
        }.resume()
    }
}

struct HomeView_Previews: PreviewProvider {
    static var previews: some View {
        HomeView(pollingManager: LongPollingManager())
    }
}
