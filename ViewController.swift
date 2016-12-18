//
//  ViewController.swift
//  mach_portal
//
//  Created by Ian Beer on 11/27/16.
//  Copyright © 2016 Ian Beer. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        DispatchQueue.main.async(execute: { () -> Void in
          jb_go();
        })
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

