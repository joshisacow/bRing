//
//  StatsView.swift
//  ece655_final
//
//  Created by Emily Shao on 11/8/24.
//

import SwiftUI

struct StatsView: View {
    var body: some View {
        
//        @State private var responseDataStats: String?
        
        VStack {
            HStack {
                Text("bRing Statistics")
                    .font(.system(size: 34, weight: .bold))
                    .foregroundColor(.white)
                    .padding(.leading)
                Spacer()
            }
            .frame(maxWidth: .infinity, minHeight: 60, idealHeight: 80)
            .background(Color.blue)

            Spacer()
            
            // Add a button to send HTTP GET request
            Button(action: {
                sendGetRequest()
            }) {
                Text("Fetch Stats")
                    .font(.system(size: 20, weight: .medium))
                    .foregroundColor(.white)
                    .padding()
                    .frame(maxWidth: .infinity)
                    .background(Color.green)
                    .cornerRadius(10)
            }
            .padding()

            // Display the response data or status message
//            if let data = responseDataStats {
//                Text(data)
//                    .font(.system(size: 18))
//                    .padding()
//            }

            Spacer()
        }
    }
    
    func sendGetRequest() {
        guard let url = URL(string: "http://172.31.84.171/stats") else { return }
        
        var request = URLRequest(url: url)
        request.httpMethod = "GET"
        
//        URLSession.shared.dataTask(with: request) { data, response, error in
//            DispatchQueue.main.async {
//                if let error = error {
//                    self.responseData = "Request errored: \(error.localizedDescription)"
//                } else if let data = data, let responseString = String(data: data, encoding: .utf8) {
//                    self.responseData = "Response: \(responseString)"
//                } else {
//                    self.responseData = "Request failed: No data received"
//                }
//            }
//        }.resume()
    }
}

struct StatsView_Previews: PreviewProvider {
    static var previews: some View {
        StatsView()
    }
}
