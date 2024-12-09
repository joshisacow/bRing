//
//  StreamingView.swift
//  ece655_final
//
//  Created by Emily Shao on 11/8/24.
//

import SwiftUI

struct StreamingView: View {
    var body: some View {
        VStack {
            HStack {
                Text("bRing View")
                    .font(.system(size: 34, weight: .bold))
                    .foregroundColor(.white)
                    .padding(.leading)
                Spacer()
            }
            .frame(maxWidth: .infinity, minHeight: 60, idealHeight: 80)
            .background(Color.blue)

            Spacer()
        }
    }
}

struct StreamingView_Previews: PreviewProvider {
    static var previews: some View {
        StreamingView()
    }
}
