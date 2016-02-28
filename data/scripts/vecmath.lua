--[[
Hyperion LUA Library
class.lua: see http://lua-users.org/wiki/SimpleLuaClasses
    Compatible with Lua 5.1 (not 5.0).
--]]


function class(base,ctor)
  local c = {}     -- a new class instance
  if not ctor and type(base) == 'function' then
      ctor = base
      base = nil
  elseif type(base) == 'table' then
   -- our new class is a shallow copy of the base class!
      for i,v in pairs(base) do
          c[i] = v
      end
      c._base = base
  end
  -- the class will be the metatable for all its objects,
  -- and they will look up their methods in it.
  c.__index = c

  -- expose a ctor which can be called by <classname>(<args>)
  local mt = {}
  mt.__call = function(class_tbl,...)
    local obj = {}
    setmetatable(obj,c)
    if ctor then
       ctor(obj,...)
    else 
    -- make sure that any stuff from the base class is initialized!
       if base and base.init then
         base.init(obj,...)
       end
    end
    return obj
  end
  c.init = ctor
  c.is_a = function(self,klass)
      local m = getmetatable(self)
      while m do 
         if m == klass then return true end
         m = m._base
      end
      return false
    end
  setmetatable(c,mt)
  return c
end

vec4 = class(function(pt,x,y,z,w)
	z=z or 0
	y=y or 0
	x=x or 0
	w=w or 1
   pt:set(x,y,z,w)
 end)
 
function vec4.set(pt,x,y,z,w) 
 if type(x) == 'table' then
     local po = x
     x = po[1]
     y = po[2]
     z = po[3]
     w = po[4]
  end
  pt[1] = x
  pt[2] = y
  pt[3] = z
  pt[4] = w
end



function vec4.__eq(p1,p2)
  return (p1[1]==p2[1]) and (p1[2]==p2[2]) and (p1[3]==p2[3]) and (p1[4]==p2[4])
end

function vec4.get(p)
  return p[1],p[2],p[3],p[4]
end

function vec4.__add(p1,p2)
  return vec4(p1[1]+p2[1], p1[2]+p2[2], p1[3]+p2[3], 1)
end

function vec4.__sub(p1,p2)
  return vec4(p1[1]-p2[1], p1[2]-p2[2], p1[3]-p2[3],1)
end

-- unitary minus  (e.g in the expression f(-p))
function vec4.__unm(p)
  return vec4(-p[1], -p[2], -p[3], p[4])
end

-- * is scalar mult or dot product)
function vec4.__mul(s,p)
	if type(s) == 'table' then
		return p1[1]*p2[1] + p1[2]*p2[2] + p1[3]*p2[3]	
	else
		return vec4( s*p[1], s*p[2], s*p[3], 1)
	end
end

function vec4.__div(p,s)
	return vec3( p[1]/s, p[2]/s, p[3]/s, 1)	
end

-- cross product is '^'
function vec4.__pow(p1,p2)
	return vec4( s[2]*p[3] - s[3]*p[2], s[3]*p[1] - s[1]*p[3], s[1]*p[2]-s[2]*p[1], 1)   
end

function vec4.normalize(p)
  local l = p:len()
  p[1] = p[1]/l
  p[2] = p[2]/l
  p[3] = p[3]/l
  p[4] = p[4]
end

function vec4.__tostring(p)	
	return string.format('(%f,%f,%f,%f)',p[1],p[2],p[3],p[4])
end

function vec4.__concat(s,p)	
	return tostring(s)..tostring(p)
end

function vec4.len(p)
	return math.sqrt(p*p)
end



mat4 = class(function(pt,m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34,m41, m42, m43, m44)
	m11=m11 or 1
	m12=m12 or 0
	m13=m13 or 0
	m14=m14 or 0
	m21=m21 or 0
	m22=m22 or 1
	m23=m23 or 0
	m24=m24 or 0
	m31=m31 or 0
	m32=m32 or 0
	m33=m33 or 1
	m34=m34 or 0
	m41=m41 or 0
	m42=m42 or 0
	m43=m43 or 0
	m44=m44 or 1		
   pt:set(m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34,m41, m42, m43, m44)
 end)
 
function mat4.set(pt,m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34,m41, m42, m43, m44)
 if type(m11) == 'table' then
     local po = m11
	 for i = 1,16,1
	 do
		pt[i] = po[i]
	 end     
  else
	  pt[1] = m11
	  pt[2] = m12
	  pt[3] = m13
	  pt[4] = m14
	  pt[5] = m21
	  pt[6] = m22
	  pt[7] = m23
	  pt[8] = m24
	  pt[9] = m31
	  pt[10] = m32
	  pt[11] = m33
	  pt[12] = m34
	  pt[13] = m41
	  pt[14] = m42
	  pt[15] = m43
	  pt[16] = m44
	end
end

function mat4.__mul(s,p)
	if type(p) == 'table' and getmetatable(p) == vec4 then		
		return vec4(s[1]*p[1] + s[2]*p[2] + s[3]*p[3] + s[4]*p[4], s[5]*p[1] + s[6]*p[2] + s[7]*p[3] + s[8]*p[4],
					s[9]*p[1] + s[10]*p[2] + s[11]*p[3] + s[12]*p[4], s[13]*p[1] + s[14]*p[2] + s[15]*p[3] + s[16]*p[4])	
	end
end

function mat4.getforward(s)
	return vec4(s[9],s[10],s[11],1)
end

function mat4.getposition(s)
	return vec4(s[13],s[14],s[15],1)
end

function mat4.setposition(s, p)
	s[13] = p[1]
	s[14] = p[2]
	s[15] = p[3]
	s[16] = p[4]
end
