static const char *coreModuleCode = 
"model None {}\n"
"model Bool {}\n"
"model Func {}\n"
"model Thread {}\n"
"\n"
"model Sequence {\n"
"   all(f) {\n"
"       auto res = true\n"
"       loop elem (this) {\n"
"           res = f.call(elem)\n"
"           if (!res) return res\n"
"       }\n"
"       return res\n"
"   }\n"
"\n"
"   any(f) {\n"
"       auto res = false\n"
"       loop elem (this) {\n"
"           res = f.call(elem)\n"
"           if (res) return res\n"
"       }\n"
"       return res\n"
"   }\n"
"\n"
"   contains(elem) {\n"
"       loop item (this) if (elem == item) return true\n"
"       return false\n"
"   }\n"
"\n"
"   count {\n"
"       auto res = 0\n"
"       loop elem (this) res = res + 1\n"
"       return res\n"
"   }\n"
"\n"
"   count(f) {\n"
"       auto res = 0\n"
"       loop elem (this) if (f.call(elem)) res = res + 1\n"
"       return res\n"
"   }\n"
"\n"
"   each(f) {\n"
"       loop elem (this) f.call(elem) \n"
"   }\n"
"\n"
"   empty {\n"
"       return iterate(none) ? false : true\n"
"   }\n"
"\n"
"   map(transformation) {\n"
"       return MapSequence.make(this, transformation)\n"
"   }\n"
"\n"
"   where(predicate) {\n"
"       return WhereSequence.make(this, predicate)\n"
"   }\n"
"\n"
"   reduce(acc, f) {\n"
"       loop elem (this) acc = f.call(acc, elem)\n"
"       return acc\n"
"   }\n"
"\n"
"   reduce(f) {\n"
"       auto iter = iterate(none)\n"
"       if (!iter) Thread.abort(\"Can not reduce an empty sequence.\")\n"
"       auto res = iteratorValue(iter)\n"
"       loop (iter = iterate(iter)) res = f.call(res, iteratorValue(iter))\n"
"       return res\n"
"   }\n"
"\n"
"   join(sep) {\n"
"       auto first = true\n"
"       auto res = \"\"\n"
"       loop elem (this) {\n"
"           if (!first) res = res + seq\n"
"           first = false\n"
"           res = res + elem.toString\n"
"       }\n"
"       return res\n"
"   }\n"
"\n"
"   join() {\n"
"       return join(\"\")\n"
"   }\n"
"\n"
"   toList {\n"
"       auto res = List.make()\n"
"       loop elem (this) res.add(elem)\n"
"       return res\n"
"   }\n"
"}\n"
"\n"
"model MapSequence inc Sequence {\n"
"   auto sequence\n"
"   auto func\n"
"   make(seq, f) {\n"
"       sequence = seq\n"
"       func = f\n"
"   }\n"
"\n"
"   iterate(iterator) {\n"
"       return sequence.iterate(iterator)\n"
"   }\n"
"\n"
"   iterateValue(iterator) {\n"
"       return func.call(sequence.iteratorValue(iterator))\n"
"   }\n"
"}\n"
"\n"
"model WhereSequence inc Sequence {\n"
"   auto sequence\n"
"   auto func\n"
"   make(seq, f) {\n"
"       sequence = seq\n"
"       func = f\n"
"   }\n"
"\n"
"   iterate(iterator) {\n"
"       loop (iterator = sequence.iterate(iterator)) \n"
"           if (func.call(sequence.iteratorValue(iterator))) break\n"
"       return iterator\n"
"   }\n"
"\n"
"   iteratorValue(iterator) {\n"
"       return sequence.iteratorValue(iterator)\n"
"   }\n"
"}\n"
"\n"
"model String inc Sequence {\n"
"   bytes {\n"
"       return StringByteSequence.make(this)\n"
"   }\n"
"\n"
"   codePoints {\n"
"       return StringCodePointSequence.make(this)\n"
"   }\n"
"\n"
"   *(count) {\n"
"       if (!(count is num) || !count.isInteger || count < 0)\n"
"           Thread.abort(\"Count must be a non-negative integer.\")\n"
"       auto res = \"\"\n"
"       loop i (0...(count - 1)) res = res + this\n"
"       return res\n"
"   }\n"
"}\n"
"\n"
"model StringByteSequence inc Sequence {\n"
"   auto string\n"
"   make(str) {\n"
"       string = str\n"
"   }\n"
"\n"
"   [index] {\n"
"       return string.byteAt_(index)\n"
"   }\n"
"\n"
"   iterate(iterator) {\n"
"       return string.iterateByte_(iterator)\n"
"   }\n"
"\n"
"   iteratorValue(iterator) {\n"
"       return string.byteAt_(iterator)\n"
"   }\n"
"\n"
"   count {\n"
"       return string.byteCount_\n"
"   }\n"
"}\n"
"\n"
"model StringCodePointSequence inc Sequence {\n"
"   auto string\n"
"   make(str) {\n"
"       string = str\n"
"   }\n"
"\n"
"   [index] {\n"
"       return string.codePointAt_(index)\n"
"   }\n"
"\n"
"   iterate(iterator) {\n"
"       return string.iterate(iterator)\n"
"   }\n"
"\n"
"   iteratorValue(iterator) {\n"
"       return string.codePointAt_(iterator)\n"
"   ]\n"
"\n"
"   count {\n"
"       return string.count\n"
"   }\n"
"}\n"
"\n"
"model List inc Sequence {\n"
"   addAll(other) {\n"
"       loop elem (other) add(elem)\n"
"       return other\n"
"   }\n"
"\n"
"   toString {\n"
"       return \"[%(join(\",\"))]\"\n"
"   }\n"
"\n"
"   +(other) {\n"
"       auto res = this(0...-1)\n"
"       loop elem (other) res.add(elem)\n"
"       return res\n"
"   }\n"
"\n"
"   *(count) {\n"
"       if (!(count is num) || !count.isInteger || count < 0)\n"
"           Thread.abort(\"count must be a non-negative interger.\")\n"
"       auto res = []\n"
"       loop i (0...(count - 1)) res.addAll(this)\n"
"       return res\n"
"   }\n"
"}\n"
"\n"
"model Map {\n"
"   keys {\n"
"       return MapKeySequence.make(this)\n"
"   }\n"
"\n"
"   values {\n"
"       return MapValueSequence.make(this)\n"
"   }\n"
"\n"
"   toString {\n"
"       auto first = true\n"
"       auto res = \"{\"\n"
"       loop key (keys) {\n"
"           if (!first) res = res + \", \"\n"
"           first = false\n"
"           res = res + \"%(key): %(this[key])\"\n"
"       }\n"
"}\n"
"\n"
"model MapKeySequence inc Sequence {\n"
"   auto map\n"
"   make(mp) {\n"
"       map = mp\n"
"   }\n"
"\n"
"   iterate(n) {\n"
"       return map.iterate_(n)\n"
"   }\n"
"\n"
"   iteratorValue(iterator) {\n"
"       return map.keyIteratorValue_(iterator)\n"
"   }\n"
"}\n"
"\n"
"model MapValueSequence inc Sequence {\n"
"   auto map\n"
"   make(mp) {\n"
"       map = mp\n"
"   }\n"
"\n"
"   iterate(n) {\n"
"       return map.iterate_(n) \n"
"   }\n"
"\n"
"   iteratorValue(iterator) {\n"
"       return map.valueIteratorValue_(iterator)\n"
"   }\n"
"}\n"
"\n"
"model Range inc Sequence {}\n"
"\n"
"model Console {\n"
"   static log() {\n"
"       writeString_(\"\n\")\n"
"   }\n"
"\n"
"   static log(obj) {\n"
"       writeObject_(obj)\n"
"       writeString_(\"\n\")\n"
"       return obj\n"
"   ]\n"
"\n"
"   static logAll(sequence) {\n"
"       loop object (sequence) writeObject_(object)\n"
"       writeString_(\"\n\")\n"
"   }\n"
"\n"
"   static write(obj) {\n"
"       writeObject_(obj)\n"
"       return obj\n"
"   }\n"
"\n"
"   static writeAll(sequence) {\n"
"       loop object (sequence) writeObject_(object)\n"
"   }\n"
"\n"
"   static writeObject_(obj) {\n"
"       auto str = obj.toString\n"
"       if (str is String) {\n"
"           writeString_(str)\n"
"       } else {\n"
"           writeString_(\"[invalid toString]\")\n"
"       }\n"
"   }\n"
"}\n";