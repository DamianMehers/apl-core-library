/**
 * Copyright 2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef _APL_IMAGE_COMPONENT_H
#define _APL_IMAGE_COMPONENT_H

#include "mediacomponenttrait.h"

namespace apl {

class ImageComponent : public CoreComponent,
                       public MediaComponentTrait {
public:
    static CoreComponentPtr create(const ContextPtr& context, Properties&& properties, const std::string& path);

    ImageComponent(const ContextPtr& context, Properties&& properties, const std::string& path);

    ComponentType getType() const override { return kComponentTypeImage; }

    void postProcessLayoutChanges() override;

protected:
    const EventPropertyMap& eventPropertyMap() const override;
    const ComponentPropDefSet& propDefSet() const override;

    std::string getVisualContextType() override;

    /// Media component trait overrides
    std::set<std::string> getSources() override;
    CoreComponentPtr getComponent() override { return shared_from_corecomponent(); }
};

} // namespace apl

#endif //_APL_IMAGE_COMPONENT_H
