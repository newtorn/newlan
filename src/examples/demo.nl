#!/usr/bin/newlan
import Phone

model iPhone inc Phone {
    auto version
    auto battery
    auto size
    static auto pNum = 0

    make(v, b, s) {
        super("iPhone")
        this.version = v
        this.battery = b
        size = s
        pNum = pNum + 1
        super.kind = super.kind + 1
    }

    size {
        return size
    }

    version=(v) {
        this.version = v
    }

    showInfo() {
        System.print("iPhone's version: %(version), battery: %(battery), size: %(size)")
    }

    static pNum() {
        System.print(pNum)
    }
}

// I want to create a iPhone which extends Phone
auto p = iPhone.make("1.0.0", 87, [320, 680])
p.showInfo()
System.print(p.size)
p.version = "2.0.1"
p.showInfo()
iPhone.pNum()

/*
These are bultin type creation
*/
auto ls = list.make()
auto mp = map.make()
auto ls1 = ["1", "2"]
auto mp1 = {"xm": 27, "zs": 20}
System.print(ls1[0])
System.print(mp1["zs"])

loop i (ls1) {
    System.print(i)
}

loop i (mp1) {
    System.print(mp1[i])
}

if (ls is none) {
    System.print(true)
}
else {
    System.print(false)
}

auto i = int("123")
auto s = str(0x3f4E5a)
auto s = str(0b1011)
auto s = str(0777)

// This is a primitive function
func sum(a, b) {
    return a + b
}

auto t = sum(12, 13)
System.print(t)

loop i (1...123456789123456789123456789123456789123456789) {
    System.print("count %(i) to end.")
}