import people inc People 
func fn() {
   var p = People.make("xiaoming", "male")
   p.sayHi()
}

model Family inc People {
   auto father
   auto mother
   auto child
   make(f, m, c) {
      father = f
      mother = m
      child  = c
      super("wbf", "male")
   }
}

auto f = Family.make("wbf", "ls", "shine")
f.sayHi()

fn() 