//
//  LoginView.swift
//  ece655_final
//
//  Created by Emily Shao on 11/8/24.
//

import SwiftUI

struct LoginView: View {
    @State private var username: String = ""
    @State private var password: String = ""
    
    var body: some View {
        VStack {
            HStack {
                Text("Login")
                    .font(.system(size: 34, weight: .bold))
                    .foregroundColor(.white)
                    .padding(.leading)
                Spacer()
            }
            .frame(maxWidth: .infinity, minHeight: 60, idealHeight: 80)
            .background(Color.blue)
            .padding(.bottom, 50)

            TextField("Username", text: $username)
                .padding()
                .background(Color.gray.opacity(0.2))
                .cornerRadius(10)
                .padding(.horizontal, 50)
            
            SecureField("Password", text: $password)
                .padding()
                .background(Color.gray.opacity(0.2))
                .cornerRadius(10)
                .padding(.horizontal, 50)
            
            HStack {
                Button("Forgot Password") {
                    // Action for Forgot Password
                }
                .foregroundColor(.blue)
                .padding(.leading)

                Spacer()
                
                Button("Login") {
                    // Action for Login
                    
                    print("Username entered: \(username)")
                    print("Username entered: \(password)")
                }
                .foregroundColor(.white)
                .padding(.horizontal, 20)
                .padding(.vertical, 10)
                .background(Color.blue)
                .cornerRadius(10)
            }
            .padding(.horizontal, 50)
            .padding(.top, 10)
            
            Spacer()
        }
    }
}

struct LoginView_Previews: PreviewProvider {
    static var previews: some View {
        LoginView()
    }
}
